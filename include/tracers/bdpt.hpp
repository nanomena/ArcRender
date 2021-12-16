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
    double t;
    scene->intersect(lB, object, t);

    Spectrum mediumMul = medium->sample(t, object, RNG);

    Vec3 intersect = lB.o + t * lB.d;

    if (object->glossy(intersect)) {
        Ray vB;
        Spectrum surfaceMul = object->sampleBxDF(lB, vB, medium, RNG);
        revTrace(idx, vB, medium, traceDepth, traceMul * mediumMul * surfaceMul, RNG);
        return;
    }

    if (object == scene->skybox) return;

#pragma omp critical
    {
        photons[traceDepth].push_back(Photon{intersect, lB, object, traceMul * mediumMul});
    };

    Ray vB;
    Spectrum surfaceMul = object->sampleBxDF(lB, vB, medium, RNG);
    revTrace(idx, vB, medium, traceDepth + 1, traceMul * mediumMul * surfaceMul, RNG);
}

Spectrum BidirectionalPathTracer::trace(
    const int &idx, const Ray &v, shared_ptr<Medium> medium, int traceDepth, Spectrum traceMul, Sampler &RNG
) {
    if (traceMul.norm() < traceEPS) return Spectrum(0);
    if (traceDepth >= traceLimit) return Spectrum(0);

    shared_ptr<Shape> object;
    double t;
    scene->intersect(v, object, t);

    Spectrum mediumMul = medium->sample(t, object, RNG);

    Vec3 intersect = v.o + t * v.d;

    Spectrum color = object->evaluateLight(v) / traceDepth;

    if (object->glossy(intersect)) {
        Ray l;
        Spectrum surfaceMul = object->sampleBxDF(v, l, medium, RNG);
        color = color + trace(idx, l, medium, traceDepth, traceMul * mediumMul * surfaceMul, RNG) * surfaceMul;

        return color * mediumMul;
    }

    if (object == scene->skybox) return color * mediumMul;

    for (const auto &light: scene->lights) {
        Ray l;
        double dis;

        Spectrum baseColor = light->sampleLight(intersect, l, dis, RNG);
        if (scene->visible(l, light, dis)) {
            color += baseColor * object->evaluateBxDF(v, l, dis) / (traceDepth + 1);
        }
    }

    for (int rev = 1; rev < traceLimit; ++rev)
        if (!photons[rev].empty()) {
            const auto &p = photons[rev][idx % photons[rev].size()];
            double dis = (p.intersect - intersect).length();
            Ray l = Ray(intersect, (p.intersect - intersect).norm()),
                vB = Ray(p.intersect, (intersect - p.intersect).norm());
            if (scene->visible(l, p.object, dis)) {
                color += p.color * object->evaluateBxDF(v, l, dis) * p.object->evaluateBxDF(p.lB, vB)
                    / (traceDepth + 1 + rev);
            }
        }

    Ray l;
    Spectrum surfaceMul = object->sampleBxDF(v, l, medium, RNG);
    color = color + trace(idx, l, medium, traceDepth + 1, traceMul * mediumMul * surfaceMul, RNG) * surfaceMul;

    return color * mediumMul;
}

#endif
#endif //ARCRENDER_INCLUDE_TRACERS_BDPT_HPP
