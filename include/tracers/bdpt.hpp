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
        const int &idx, const Ray &lB, shared_ptr<Medium> medium, int traceDepth, Spectrum traceMul, Sampler &RNG
    );
    Spectrum trace(
        const int &idx, const Ray &v, shared_ptr<Medium> medium, int traceDepth, Spectrum traceMul, Sampler &RNG
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
    Spectrum traceMul = scene->lights[idx % scene->lights.size()]->sampleLight(lB, medium, RNG);
    revTrace(idx, lB, medium, 1, traceMul, RNG);
}

void BidirectionalPathTracer::sample(int idx, Sampler &RNG) {
    Ray v = sampleCamera(idx, RNG);
    Spectrum result = trace(idx, v, scene->medium, 1, Spectrum(1), RNG);

#pragma omp critical
    {
        C[idx] += result;
        W[idx] += 1;
    };
}

void BidirectionalPathTracer::revTrace(
    const int &idx, const Ray &lB, shared_ptr<Medium> medium, int traceDepth, Spectrum traceMul, Sampler &RNG
) {
    if (traceMul.norm() < traceEPS) return;
    if (traceDepth >= traceLimit) return;

    shared_ptr<Shape> object;
    Vec3 intersect; Ray lBt;
    Spectrum mediumMul = medium->sample(scene, lB, lBt, object, intersect, RNG);

    if (object->glossy(intersect)) {
        Ray vB;
        Spectrum surfaceMul = object->sampleBxDF(lBt, vB, medium, RNG);
        revTrace(idx, vB, medium, traceDepth, traceMul * mediumMul * surfaceMul, RNG);
        return;
    }

    if (object == scene->skybox) return;

#pragma omp critical
    {
        photons[traceDepth].push_back(Photon{intersect, lBt, object, traceMul * mediumMul});
    };

    Ray vB;
    Spectrum surfaceMul = object->sampleBxDF(lBt, vB, medium, RNG);
    revTrace(idx, vB, medium, traceDepth + 1, traceMul * mediumMul * surfaceMul, RNG);
}

Spectrum BidirectionalPathTracer::trace(
    const int &idx, const Ray &v, shared_ptr<Medium> medium, int traceDepth, Spectrum traceMul, Sampler &RNG
) {
    if (traceMul.norm() < traceEPS) return Spectrum(0);
    if (traceDepth >= traceLimit) return Spectrum(0);

    shared_ptr<Shape> object;
    Vec3 intersect; Ray vt;
    Spectrum mediumMul = medium->sample(scene, v, vt, object, intersect, RNG);

    Spectrum color = object->evaluateLight(vt) / traceDepth;

    if (object->glossy(intersect)) {
        Ray l;
        Spectrum surfaceMul = object->sampleBxDF(vt, l, medium, RNG);
        color += trace(idx, l, medium, traceDepth, traceMul * mediumMul * surfaceMul, RNG) * surfaceMul;

        return color * mediumMul;
    }

    if (object == scene->skybox) return color * mediumMul;

    for (const auto &light: scene->lights) {
        Vec3 pos;
        double pdf;
        light->sample(pos, pdf, RNG);
        color += medium->evaluate(
            scene, vt, object, intersect, light, pos, [&](const Vec3 &view) {
                return light->evaluateLightBack({pos, (view - pos).norm()});
            }, RNG
        ) / pdf / (traceDepth + 1);
        shared_ptr<Medium> other = object->otherSide(medium);
        if (other == nullptr) continue;
        color += other->evaluate(
            scene, vt, object, intersect, light, pos, [&](const Vec3 &view) {
                return light->evaluateLightBack({pos, (view - pos).norm()});
            }, RNG
        ) / pdf / (traceDepth + 1);
    }

    for (int rev = 1; rev < traceLimit; ++rev)
        if (!photons[rev].empty()) {
            const auto &p = photons[rev][idx % photons[rev].size()];
            color += medium->evaluate(
                scene, vt, object, intersect, p.object, p.intersect, [&](const Vec3 &view) {
                    return p.color * p.object->evaluateBxDF(p.lB, {p.intersect, (view - p.intersect).norm()});
                }, RNG
            ) / (traceDepth + 1 + rev);
            shared_ptr<Medium> other = object->otherSide(medium);
            if (other == nullptr) continue;
            color += other->evaluate(
                scene, vt, object, intersect, p.object, p.intersect, [&](const Vec3 &view) {
                    return p.color * p.object->evaluateBxDF(p.lB, {p.intersect, (view - p.intersect).norm()});
                }, RNG
            ) / (traceDepth + 1 + rev);
        }

    Ray l;
    Spectrum surfaceMul = object->sampleBxDF(vt, l, medium, RNG);
    color += trace(idx, l, medium, traceDepth + 1, traceMul * mediumMul * surfaceMul, RNG) * surfaceMul;

    return color * mediumMul;
}

#endif
#endif //ARCRENDER_INCLUDE_TRACERS_BDPT_HPP
