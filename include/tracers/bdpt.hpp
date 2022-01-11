#ifndef ARCRENDER_INCLUDE_TRACERS_BDPT_HPP
#define ARCRENDER_INCLUDE_TRACERS_BDPT_HPP

#include "../tracer.hpp"

struct BiPdf {
    double last, sum2;

    BiPdf () : last(0), sum2(0) {}
    BiPdf (double last, double sum2) : last(last), sum2(sum2) {}
};

struct Photon {
    Vec3 pos;
    Vec2 texPos;
    Ray lB;
    const Object *object;
    BiPdf pdf;
    Spectrum color;
};

class BidirectionalPathTracer : public Tracer {
public:
    void epoch() override;

    void revTrace(
        Photon photonBuffer[], const Ray &lB, const Medium *medium, int traceDepth, Spectrum traceMul,
        BiPdf pdf,
        Sampler &RNG
    );
    Spectrum trace(
        Photon photonBuffer[], const Ray &v, const Medium *medium, int traceDepth, Spectrum traceMul, BiPdf pdf,
        Sampler &RNG
    );

    BidirectionalPathTracer(
        int width, int height, const Scene *scene, int traceLimit = 20, double traceEps = 1e-7
    );

private:
    int traceLimit;
    double traceEPS;
};

#ifdef ARC_IMPLEMENTATION

BidirectionalPathTracer::BidirectionalPathTracer(
    int width, int height, const Scene *scene, int traceLimit, double traceEPS
) : Tracer(width, height, scene), traceLimit(traceLimit), traceEPS(traceEPS) {}


void BidirectionalPathTracer::epoch() {
    Sampler RNGs[MaxThreads];
    Photon photonBuffers[MaxThreads][traceLimit];
    pair<int, Spectrum> spectrumBuffers[MaxThreads][traceLimit];

    vector<double> lightImportance;
    for (auto light : scene->lights) {
        Spectrum sum;
        for (int i = 0; i < width + height; ++ i) {
            Ray lB;
            Vec3 pos; Vec2 texPos;
            const Medium *medium;
            sum += light->sampleLight(lB.d, pos, texPos, medium, RNGs[0]);
        }
        lightImportance.push_back(sum.norm());
    }
    {
        double tot = 0;
        for (double i : lightImportance) tot += i;
        for (double &l : lightImportance) l /= tot;
    }

#pragma omp parallel for schedule(dynamic, width) default(none) \
    shared(spectrum, number) firstprivate(RNGs, photonBuffers, spectrumBuffers, lightImportance)
    for (int idx = 0; idx < length; idx++) {
        int thd = omp_get_thread_num();
        auto &RNG = RNGs[thd];
        auto photonBuffer = photonBuffers[thd];
        auto spectrumBuffer = spectrumBuffers[thd];

        for (int rev = 1; rev < traceLimit; ++rev) {
            photonBuffer[rev] = {Vec3(), Vec2(), Ray(), nullptr, BiPdf(), Spectrum()};
            spectrumBuffer[rev] = make_pair(-1, Spectrum(0));
        }
        {
            Ray lB;
            const Medium *medium;
            double pt = RNGs[thd].rand();
            int lightIdx = 0;
            while (lightIdx + 1 < scene->lights.size() && pt > lightImportance[lightIdx])
                pt -= lightImportance[lightIdx ++];
            const Shape *light = scene->lights[lightIdx];
            Vec3 pos; Vec2 texPos;
            Spectrum traceMul = light->sampleLight(lB.d, pos, texPos, medium, RNG) / lightImportance[lightIdx];
            lB.o = pos;
            revTrace(
                photonBuffer, lB, medium, 1, traceMul, {
                    light->evaluateLightImportance(lB.d, pos, texPos) * light->evaluateSurfaceImportance(pos, texPos),
                    1 / light->evaluateSurfaceImportance(pos, texPos)
                }, RNG
            );
        }
        for (int rev = 1; rev < traceLimit; ++rev) {
            auto pos = photonBuffer[rev].pos;
            auto texPos = photonBuffer[rev].texPos;
            auto object = photonBuffer[rev].object;
            auto pdf = photonBuffer[rev].pdf;
            auto lB = photonBuffer[rev].lB;
            auto color = photonBuffer[rev].color;

            if (object == nullptr) continue;
            Vec2 cameraV2;
            Ray v;
            double cameraWeight = scene->camera->evaluate(pos, v, cameraV2, RNG);
            if (cameraWeight <= 0) continue;
            int cameraIdx = getCameraIdx(cameraV2);
            if (cameraIdx == -1) continue;
            double t = (v.o - pos).length();
            Ray vB = {pos, (v.o - pos).norm()};

            const Medium *curMedium = scene->visible(v, object, t);
            if (curMedium == nullptr) continue;
            double pdfSum2b = (pdf.sum2 * pow(object->evaluateBxDFImportance(-vB.d, pos, texPos, -lB.d), 2)
                    + (vB.o - lB.o).squaredLength())
                * pow(1. / t / pdf.last, 2);

            Spectrum result = curMedium->evaluate(v, t) * object->evaluateBxDF(lB.d, pos, texPos, vB.d) * color
                / pow(t, 2) / (1 + pdfSum2b) * cameraWeight;
            if (result.r != result.r) {
                assert(0);
                exit(-1);
            }
            spectrumBuffer[rev] = make_pair(cameraIdx, result);
        }
        {
            Ray v;
            sampleCamera(v, idx, RNG);
            Spectrum result = trace(photonBuffer, v, scene->medium, 1, Spectrum(1), {1, 0}, RNG);
//            if (result.r != result.r) {
//                assert(0);
//                exit(-1);
//            }
#pragma omp critical
            {
                number[idx] += 1;
                spectrum[idx] += result;
                for (int rev = 1; rev < traceLimit; ++rev) if (spectrumBuffer[rev].first >= 0) {
                    spectrum[spectrumBuffer[rev].first] += spectrumBuffer[rev].second;
                }
            };
        }
    }
}

void BidirectionalPathTracer::revTrace(
    Photon photonBuffer[], const Ray &lB, const Medium *medium, int traceDepth, Spectrum traceMul, BiPdf pdf,
    Sampler &RNG
) {
    if (traceMul.norm() < .05) {
        if (RNG.sample() < traceMul.norm() * 10) traceMul *= 1 / (10 * traceMul.norm());
        else return;
    }
    if (traceDepth >= traceLimit) return;

    const Object *object;
    Vec3 pos; Vec2 texPos;
    Spectrum mediumMul = medium->sample(scene, lB, object, pos, texPos, RNG);

    if (object == scene->skybox) return;

    photonBuffer[traceDepth] = {pos, texPos, lB, object, pdf, traceMul * mediumMul};

    Ray vB;
    Spectrum surfaceMul = object->sampleBxDF(lB.d, pos, texPos, vB.d, medium, RNG);
    vB.o = pos;

    double pdfSum2 = (pdf.sum2 * pow(object->evaluateBxDFImportance(-vB.d, pos, texPos, -lB.d), 2)
                + (vB.o - lB.o).squaredLength())
            / pow(pdf.last, 2);

//    cerr << " G G " << endl;
//    cerr << (pdf.sum2 * pow(object->evaluateBxDFPdf({vB.o + vB.d, -vB.d}, {vB.o, -lB.d}), 2)
//        + (vB.o - lB.o).squaredLength()) << " " << pow(pdf.last, 2) << endl;
//    cerr << object->evaluateBxDFPdf(lB, vB) << " " << pdfSum2 << endl;

    revTrace(photonBuffer, vB, medium, traceDepth + 1, traceMul * mediumMul * surfaceMul,
        {object->evaluateBxDFImportance(lB.d, pos, texPos, vB.d), pdfSum2}, RNG);
}

Spectrum BidirectionalPathTracer::trace(
    Photon photonBuffer[], const Ray &v, const Medium *medium, int traceDepth, Spectrum traceMul, BiPdf pdf, Sampler &RNG
) {
    Spectrum extraMul(1);
    if (traceMul.norm() < .05) {
        if (RNG.sample() < traceMul.norm() * 10) extraMul = Spectrum(1 / (10 * traceMul.norm()));
        else return Spectrum(0);
    }
    if (traceDepth >= traceLimit) return Spectrum(0);

    const Object *object;
    Vec3 pos; Vec2 texPos;
    Spectrum mediumMul = medium->sample(scene, v, object, pos, texPos, RNG);

    if (object == scene->skybox) return scene->skyboxColor * mediumMul * extraMul;

    Spectrum color{0};
    if (object->isLight()) {
        double pdfSum2f = (pdf.sum2 * pow(object->evaluateLightImportance(-v.d, pos, texPos), 2)
                + (pos - v.o).squaredLength())
            * pow(object->evaluateSurfaceImportance(pos, texPos) / pdf.last, 2);
        if (traceDepth == 1) pdfSum2f = 0;
        color = object->evaluateLight(v.d, pos, texPos) / (pdfSum2f + 1);
    }

    for (const auto &light: scene->lights) {
        Vec3 LPos; Vec2 LPosTex;
        double LPosPdf;
        light->sampleSurface(LPos, LPosTex, LPosPdf, RNG);
        double t = (LPos - pos).length();
        Ray l = {pos, (LPos - pos).norm()},
            vB = {LPos, (pos - LPos).norm()};
        const Medium *curMedium = scene->visible(l, light, t);

        if (curMedium != nullptr) {
            double pdfSum2f = (pdf.sum2 * pow(object->evaluateBxDFImportance(-l.d, pos, texPos, -v.d), 2)
                    + (l.o - v.o).squaredLength())
                * pow(light->evaluateLightImportance(vB.d, LPos, LPosTex)
                * light->evaluateSurfaceImportance(LPos, LPosTex) / t / pdf.last, 2);
            double pdfSub2b = pow(object->evaluateBxDFImportance(v.d, pos, texPos, l.d) / t / light->evaluateSurfaceImportance(pos, texPos), 2);

            // cerr << pdfSum2 << endl;
            color += curMedium->evaluate(l, t) * object->evaluateBxDF(v.d, pos, texPos, l.d)
                * light->evaluateLightBack(vB.d, LPos, LPosTex) / LPosPdf / pow(t, 2)
                / (pdfSum2f + 1 + pdfSub2b);
            // / traceDepth;
        }
    }

    for (int rev = 1; rev + traceDepth < traceLimit; ++rev) if (RNG.sample() < 1. / (rev + traceDepth)) {
        const auto &p = photonBuffer[rev];

        if (p.color.norm() < traceEPS) continue;
        double t = (p.pos - pos).length();
        Ray l = {pos, (p.pos - pos).norm()},
            vB = {p.pos, (pos - p.pos).norm()};
        const Medium *curMedium = scene->visible(l, p.object, t);

        if (curMedium != nullptr) {
            double pdfSum2f = (pdf.sum2 * pow(object->evaluateBxDFImportance(-l.d, pos, texPos, -v.d), 2)
                + (l.o - v.o).squaredLength()) * pow(p.object->evaluateBxDFImportance(p.lB.d, p.pos, p.texPos, vB.d) / t / pdf.last, 2);
            double pdfSum2b = (p.pdf.sum2 * pow(p.object->evaluateBxDFImportance(-vB.d, p.pos, p.texPos, -p.lB.d), 2)
                + (vB.o - p.lB.d).squaredLength()) * pow(object->evaluateBxDFImportance(v.d, pos, texPos, l.d) / t / p.pdf.last, 2);

        // cerr << pdfSum2f << " " << pdfSum2b << endl;
            color += curMedium->evaluate(l, t) * object->evaluateBxDF(v.d, pos, texPos, l.d) * p.color
                * p.object->evaluateBxDF(p.lB.d, p.pos, p.texPos, vB.d) / pow(t, 2)
                / (pdfSum2f + 1 + pdfSum2b)
                * (rev + traceDepth);
            // / (traceDepth + rev);
        }
    }

    Ray l;
    Spectrum surfaceMul = object->sampleBxDF(v.d, pos, texPos, l.d, medium, RNG);
    l.o = pos;

    double pdfSum2 = (pdf.sum2 * pow(object->evaluateBxDFImportance(-l.d, pos, texPos, -v.d), 2)
        + (l.o - v.o).squaredLength()) / pow(pdf.last, 2);

    color += trace(photonBuffer, l, medium, traceDepth + 1, traceMul * mediumMul * surfaceMul,
        {object->evaluateBxDFImportance(v.d, pos, texPos, l.d), pdfSum2}, RNG) * surfaceMul;

    return color * mediumMul * extraMul;
}

#endif
#endif //ARCRENDER_INCLUDE_TRACERS_BDPT_HPP