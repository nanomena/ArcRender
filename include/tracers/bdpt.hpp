#ifndef ARCRENDER_INCLUDE_TRACERS_BDPT_HPP
#define ARCRENDER_INCLUDE_TRACERS_BDPT_HPP

#include "../tracer.hpp"
#include "../photon.hpp"

class BidirectionalPathTracer : public Tracer {
public:
    void initGraph(int preCnt) override;
    void preSample(int idx, Sampler &RNG) override;
    void sample(int idx, Sampler &RNG) override;

    void revTrace(
        const int &idx, const Ray &lB, shared_ptr<Medium> medium, int traceDepth, Spectrum traceMul, double lastPdf, double pdfSum2,
        Sampler &RNG
    );
    Spectrum trace(
        const int &idx, const Ray &v, shared_ptr<Medium> medium, int traceDepth, Spectrum traceMul, double lastPdf, double pdfSum2,
        Sampler &RNG
    );

    BidirectionalPathTracer(
        int width, int height, const shared_ptr<Scene> &scene, int traceLimit = 6, double traceEps = 1e-7
    );

private:
    vector<vector<Photon>> photons;
    int traceLimit;
    double traceEPS;
};

#ifdef ARC_IMPLEMENTATION

BidirectionalPathTracer::BidirectionalPathTracer(
    int width, int height, const shared_ptr<Scene> &scene, int traceLimit, double traceEPS
) : Tracer(width, height, scene), traceLimit(traceLimit), traceEPS(traceEPS) {}

void BidirectionalPathTracer::initGraph(int preCnt) {
    photons.clear();
    photons.resize(traceLimit);
}

void BidirectionalPathTracer::preSample(int idx, Sampler &RNG) {
    Ray lB;
    shared_ptr<Medium> medium;
    shared_ptr<Shape> light = scene->lights[idx % scene->lights.size()];
    Spectrum traceMul = light->sampleLight(lB, medium, RNG);
    revTrace(idx, lB, medium, 1, traceMul, light->evaluateLightPdf(lB), 0, RNG);
}

void BidirectionalPathTracer::sample(int idx, Sampler &RNG) {
    Ray v = sampleCamera(idx, RNG);
    Spectrum result = trace(idx, v, scene->medium, 1, Spectrum(1), INF, 0, RNG);

#pragma omp critical
    {
        C[idx] += result;
        W[idx] += 1;
    };
}

void BidirectionalPathTracer::revTrace(
    const int &idx, const Ray &lB, shared_ptr<Medium> medium, int traceDepth, Spectrum traceMul, double lastPdf, double pdfSum2,
    Sampler &RNG
) {
    if (traceMul.norm() < traceEPS) return;
    if (traceDepth >= traceLimit) return;

    shared_ptr<Shape> object;
    Vec3 intersect;
    Spectrum mediumMul = medium->sample(scene, lB, object, intersect, RNG);

    if (object == scene->skybox) return;

#pragma omp critical
    {
        photons[traceDepth].push_back(Photon{intersect, lB, object, lastPdf, pdfSum2, traceMul * mediumMul});
    };

    Ray vB;
    Spectrum surfaceMul = object->sampleBxDF(lB, vB, medium, RNG);

    double pdfSum2b = (pdfSum2 * pow(object->evaluateBxDFPdf({vB.o + vB.d, -vB.d}, {vB.o, -lB.d}), 2)
        + (vB.o - lB.o).squaredLength()) / pow(lastPdf, 2);

    revTrace(idx, vB, medium, traceDepth + 1, traceMul * mediumMul * surfaceMul,
        object->evaluateBxDFPdf(lB, vB), pdfSum2b, RNG);
}

Spectrum BidirectionalPathTracer::trace(
    const int &idx, const Ray &v, shared_ptr<Medium> medium, int traceDepth, Spectrum traceMul, double lastPdf, double pdfSum2, Sampler &RNG
) {
    if (traceMul.norm() < traceEPS) return Spectrum(0);
    if (traceDepth >= traceLimit) return Spectrum(0);

    shared_ptr<Shape> object;
    Vec3 intersect;
    Spectrum mediumMul = medium->sample(scene, v, object, intersect, RNG);

    Spectrum color{0};
    if (traceDepth == 1) color = object->evaluateLight(v);

    if (object == scene->skybox) return color * mediumMul;

    for (const auto &light: scene->lights) {
        Vec3 pos;
        double pdf;
        light->sample(pos, pdf, RNG);
        double t = (pos - intersect).length();
        Ray l = {intersect, (pos - intersect).norm()},
            vB = {pos, (intersect - pos).norm()};
        shared_ptr<Medium> curMedium = scene->visible(l, light, t);

        double pdfSum2n = (pdfSum2 * pow(object->evaluateBxDFPdf({l.o + l.d, -l.d}, {l.o, -v.d}), 2)
            + (l.o - v.o).squaredLength()) / pow(t, 2) * pow(light->evaluateLightPdf(vB), 2) / pow(lastPdf, 2);
        if (curMedium != nullptr) {
            color += curMedium->evaluate(t) * object->evaluateBxDF(v, l)
                * light->evaluateLightBack(vB) / pdf / pow(t, 2)
                / traceDepth;
        }
    }

    for (int rev = 1; rev < traceLimit; ++rev)
        if (!photons[rev].empty()) {
            const auto &p = photons[rev][idx % photons[rev].size()];

            double t = (p.intersect - intersect).length();
            Ray l = {intersect, (p.intersect - intersect).norm()},
                vB = {p.intersect, (intersect - p.intersect).norm()};
            shared_ptr<Medium> curMedium = scene->visible(l, p.object, t);

            double pdfSum2n = (pdfSum2 * pow(object->evaluateBxDFPdf({l.o + l.d, -l.d}, {l.o, -v.d}), 2)
                + (l.o - v.o).squaredLength()) * pow(p.object->evaluateBxDFPdf(p.lB, vB), 2) / pow(t, 2)  / pow(lastPdf, 2);
            double pdfSum2b = (p.pdfSum2 * pow(p.object->evaluateBxDFPdf({vB.o + vB.d, -vB.d}, {vB.o, -p.lB.d}), 2)
                + (vB.o - p.lB.d).squaredLength()) * pow(object->evaluateBxDFPdf(v, l), 2) / pow(t, 2) / pow(p.lastPdf, 2);
            if (curMedium != nullptr) {
                color += curMedium->evaluate(t) * object->evaluateBxDF(v, l) * p.color
                    * p.object->evaluateBxDF(p.lB, vB) / pow(t, 2)
                    / (traceDepth + rev);
            }
        }

    Ray l;
    Spectrum surfaceMul = object->sampleBxDF(v, l, medium, RNG);

    double pdfSum2n = (pdfSum2 * pow(object->evaluateBxDFPdf({l.o + l.d, -l.d}, {l.o, -v.d}), 2)
        + (l.o - v.o).squaredLength()) / pow(lastPdf, 2);

    color += trace(idx, l, medium, traceDepth + 1, traceMul * mediumMul * surfaceMul,
        object->evaluateBxDFPdf(v, l), pdfSum2n, RNG) * surfaceMul;

    return color * mediumMul;
}

#endif
#endif //ARCRENDER_INCLUDE_TRACERS_BDPT_HPP
