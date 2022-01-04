#ifndef ARCRENDER_INCLUDE_TRACERS_BDPT_HPP
#define ARCRENDER_INCLUDE_TRACERS_BDPT_HPP

#include "../tracer.hpp"

struct BiPdf {
    double last, sum2;

    BiPdf () : last(0), sum2(0) {}
    BiPdf (double last, double sum2) : last(last), sum2(sum2) {}
};

struct Photon {
    Vec3 intersect;
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

#pragma omp parallel for schedule(dynamic, width) default(none) shared(spectrum, number) firstprivate(RNGs, photonBuffers, spectrumBuffers)
    for (int idx = 0; idx < length; idx++) {
        int thd = omp_get_thread_num();
        auto &RNG = RNGs[thd];
        auto photonBuffer = photonBuffers[thd];
        auto spectrumBuffer = spectrumBuffers[thd];

        for (int rev = 1; rev < traceLimit; ++rev) {
            photonBuffer[rev] = {Vec3(), Ray(), nullptr, BiPdf(), Spectrum()};
            spectrumBuffer[rev] = make_pair(-1, Spectrum(0));
        }
        {
            Ray lB;
            const Medium *medium;
            const Shape *light = scene->lights[idx % scene->lights.size()];
            Spectrum traceMul = light->sampleLight(lB, medium, RNG) * scene->lights.size();
            revTrace(
                photonBuffer, lB, medium, 1, traceMul, {
                    light->evaluateLightImportance(lB) * light->evaluateSurfaceImportance(lB.o),
                    1 / light->evaluateSurfaceImportance(lB.o)
                }, RNG
            );
        }
        for (int rev = 1; rev < traceLimit; ++rev) {
            auto intersect = photonBuffer[rev].intersect;
            auto object = photonBuffer[rev].object;
            auto pdf = photonBuffer[rev].pdf;
            auto lB = photonBuffer[rev].lB;
            auto color = photonBuffer[rev].color;

            if (object == nullptr) continue;
            Vec2 cameraV2;
            Ray v = scene->camera->evaluate(intersect, cameraV2);
            int cameraIdx = getCameraIdx(cameraV2);
            double t = (v.o - intersect).length();
            Ray vB = {intersect, (v.o - intersect).norm()};

            const Medium *curMedium = scene->visible(v, object, t);
            if (cameraIdx != -1 && curMedium != nullptr) {
                double pdfSum2b = (
                    pdf.sum2 * pow(
                        object->evaluateBxDFImportance(
                            {vB.o + vB.d, -vB.d},
                            {vB.o, -lB.d}
                        ), 2
                    ) + (vB.o - lB.o).squaredLength())
                    * pow(scene->camera->evaluateImportance(intersect) / t / pdf.last, 2);

                Spectrum result = curMedium->evaluate(t) * object->evaluateBxDF(lB, vB) * color
                    / pow(t, 2) / (1 + pdfSum2b);
                if (result.r != result.r) {
                    exit(-1);
                }
                spectrumBuffer[rev] = make_pair(cameraIdx, result);
            }
        }
        {
            double pdf;
            Ray v = sampleCamera(idx, pdf, RNG);
            Spectrum result = trace(photonBuffer, v, scene->medium, 1, Spectrum(1), {pdf, 0}, RNG);
            if (result.r != result.r) {
                exit(-1);
            }
//#pragma omp critical
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
    Vec3 intersect;
    Spectrum mediumMul = medium->sample(scene, lB, object, intersect, RNG);

    if (object == scene->skybox) return;

    photonBuffer[traceDepth] = {intersect, lB, object, pdf, traceMul * mediumMul};

    Ray vB;
    Spectrum surfaceMul = object->sampleBxDF(lB, intersect, vB, medium, RNG);

    double pdfSum2 = (pdf.sum2 * pow(object->evaluateBxDFImportance(
            {vB.o + vB.d, -vB.d},
            {vB.o, -lB.d}), 2) + (vB.o - lB.o).squaredLength())
            / pow(pdf.last, 2);

//    cerr << " G G " << endl;
//    cerr << (pdf.sum2 * pow(object->evaluateBxDFPdf({vB.o + vB.d, -vB.d}, {vB.o, -lB.d}), 2)
//        + (vB.o - lB.o).squaredLength()) << " " << pow(pdf.last, 2) << endl;
//    cerr << object->evaluateBxDFPdf(lB, vB) << " " << pdfSum2 << endl;

    revTrace(photonBuffer, vB, medium, traceDepth + 1, traceMul * mediumMul * surfaceMul,
        {object->evaluateBxDFImportance(lB, vB), pdfSum2}, RNG);
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
    Vec3 intersect;
    Spectrum mediumMul = medium->sample(scene, v, object, intersect, RNG);

    if (object == scene->skybox) return scene->skyboxColor * mediumMul * extraMul;

    Spectrum color{0};
    if (object->isLight()) {
        double pdfSum2f = (pdf.sum2 * pow(object->evaluateLightImportance(
                {intersect, -v.d}), 2) + (intersect - v.o).squaredLength())
            * pow(object->evaluateSurfaceImportance(intersect) / pdf.last, 2);
        if (traceDepth == 1) pdfSum2f = 0;
        color = object->evaluateLight(v, intersect) / (pdfSum2f + 1);
    }

    for (const auto &light: scene->lights) {
        Vec3 pos;
        double posPdf;
        light->sampleSurface(pos, posPdf, RNG);
        double t = (pos - intersect).length();
        Ray l = {intersect, (pos - intersect).norm()},
            vB = {pos, (intersect - pos).norm()};
        const Medium *curMedium = scene->visible(l, light, t);

        if (curMedium != nullptr) {
            double pdfSum2f = (pdf.sum2 * pow(object->evaluateBxDFImportance(
                    {l.o + l.d, -l.d},
                    {l.o, -v.d}), 2) + (l.o - v.o).squaredLength())
                * pow(light->evaluateLightImportance(vB) * light->evaluateSurfaceImportance(vB.o) / t / pdf.last, 2);
            double pdfSub2b = pow(object->evaluateBxDFImportance(v, l) / t / light->evaluateSurfaceImportance(pos), 2);

            // cerr << pdfSum2 << endl;
            color += curMedium->evaluate(t) * object->evaluateBxDF(v, l)
                * light->evaluateLightBack(vB) / posPdf / pow(t, 2)
                / (pdfSum2f + 1 + pdfSub2b);
            // / traceDepth;
        }
    }

    for (int rev = 1; rev + traceDepth < traceLimit; ++rev) if (RNG.sample() < 1. / (rev + traceDepth)) {
        const auto &p = photonBuffer[rev];

        if (p.color.norm() < traceEPS) continue;
        double t = (p.intersect - intersect).length();
        Ray l = {intersect, (p.intersect - intersect).norm()},
            vB = {p.intersect, (intersect - p.intersect).norm()};
        const Medium *curMedium = scene->visible(l, p.object, t);

        if (curMedium != nullptr) {
            double pdfSum2f = (pdf.sum2 * pow(object->evaluateBxDFImportance(
                    {l.o + l.d, -l.d},
                    {l.o, -v.d}), 2)
                + (l.o - v.o).squaredLength()) * pow(p.object->evaluateBxDFImportance(p.lB, vB) / t / pdf.last, 2);
            double pdfSum2b = (p.pdf.sum2 * pow(p.object->evaluateBxDFImportance(
                    {vB.o + vB.d, -vB.d},
                    {vB.o, -p.lB.d}), 2)
                + (vB.o - p.lB.d).squaredLength()) * pow(object->evaluateBxDFImportance(v, l) / t / p.pdf.last, 2);

        // cerr << pdfSum2f << " " << pdfSum2b << endl;
            color += curMedium->evaluate(t) * object->evaluateBxDF(v, l) * p.color
                * p.object->evaluateBxDF(p.lB, vB) / pow(t, 2)
                / (pdfSum2f + 1 + pdfSum2b)
                * (rev + traceDepth);
            // / (traceDepth + rev);
        }
    }

    Ray l;
    Spectrum surfaceMul = object->sampleBxDF(v, intersect, l, medium, RNG);

    double pdfSum2 = (pdf.sum2 * pow(object->evaluateBxDFImportance(
            {l.o + l.d, -l.d},
            {l.o, -v.d}), 2)
        + (l.o - v.o).squaredLength()) / pow(pdf.last, 2);

    color += trace(photonBuffer, l, medium, traceDepth + 1, traceMul * mediumMul * surfaceMul,
        {object->evaluateBxDFImportance(v, l), pdfSum2}, RNG) * surfaceMul;

    return color * mediumMul * extraMul;
}

#endif
#endif //ARCRENDER_INCLUDE_TRACERS_BDPT_HPP