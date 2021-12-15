#ifndef ARCRENDER_INCLUDE_TRACERS_BDPT_HPP
#define ARCRENDER_INCLUDE_TRACERS_BDPT_HPP

#include "../tracer.hpp"
#include "../photon.hpp"

class BidirectionalPathTracer : public Tracer {
public:
    void initGraph(int preCnt) override;
    void preSample(int idx) override;
    void sample(int idx) override;

    void revTrace(const int &idx, const Ray &lB, const shared_ptr<Medium> &medium, int traceDepth, Spectrum traceMul);
    Spectrum trace(const int &idx, const Ray &v, const shared_ptr<Medium> &medium, int traceDepth, Spectrum traceMul);

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
) : Tracer(width, height, scene, traceLimit, traceLimit), traceLimit(traceLimit), traceEPS(traceEPS) {}

void BidirectionalPathTracer::initGraph(int preCnt) {
    photons.clear();
    photons.resize(traceLimit);
}

void BidirectionalPathTracer::preSample(int idx) {
    Ray lB;
    Spectrum traceMul = scene->lights[idx % scene->lights.size()]->sample(lB);
    revTrace(idx, lB, scene->medium, 1, traceMul);
}

void BidirectionalPathTracer::sample(int idx) {
    Ray v = sampleCamera(idx);
    C[idx] += trace(idx, v, scene->medium, 1, Spectrum(1));
    W[idx] += 1;
}

void BidirectionalPathTracer::revTrace(
    const int &idx, const Ray &lB, const shared_ptr<Medium> &medium, int traceDepth, Spectrum traceMul
) {
    if (traceMul.norm() < traceEPS) return;
    if (traceDepth >= traceLimit) return;

    shared_ptr<Shape> object;
    double t;
    scene->intersect(lB, object, t);

    double tM;
    shared_ptr<Shape> objectM = medium->sample(tM);
    if (tM < t) object = objectM, t = tM;

    Vec3 intersect = lB.o + t * lB.d;
    Spectrum mediumMul = medium->evaluate(t);

    if (object->glossy(intersect)) {
        Ray vB;
        Spectrum surfaceMul = object->sample(lB, vB);
        revTrace(idx, vB, medium, traceDepth, traceMul * mediumMul * surfaceMul);
        return;
    }
    photons[traceDepth].push_back(Photon{intersect, lB, object, traceMul * mediumMul});

    Ray vB;
    Spectrum surfaceMul = object->sample(lB, vB);
    revTrace(idx, vB, medium, traceDepth + 1, traceMul * mediumMul * surfaceMul);
}

Spectrum BidirectionalPathTracer::trace(
    const int &idx, const Ray &v, const shared_ptr<Medium> &medium, int traceDepth, Spectrum traceMul
) {
    if (traceMul.norm() < traceEPS) return Spectrum(0);
    if (traceDepth >= traceLimit) return Spectrum(0);

    shared_ptr<Shape> object;
    double t;
    scene->intersect(v, object, t);

    double tM;
    shared_ptr<Shape> objectM = medium->sample(tM);
    if (tM < t) object = objectM, t = tM;

    Vec3 intersect = v.o + t * v.d;

    Spectrum mediumMul = medium->evaluate(t);
    Spectrum color = object->evaluate(v) / traceDepth;

    if (object->glossy(intersect)) {
        Ray l;
        Spectrum surfaceMul = object->sample(v, l);
        color = color + trace(idx, l, medium, traceDepth, traceMul * mediumMul * surfaceMul) * surfaceMul;

        return color * mediumMul;
    }

    for (const auto &light: scene->lights) {
        Ray l;
        double dis;

        Spectrum baseColor = light->sample(intersect, l, dis);
        if (scene->visible(l, light, dis)) {
            color += baseColor * object->evaluate(v, l) / pow(dis, 2) * medium->evaluate(dis) / (traceDepth + 1);
        }
    }

    for (int rev = 1; rev < traceLimit; ++rev) if (!photons[rev].empty()) {
        const auto &p = photons[rev][idx % photons[rev].size()];
        double dis = (p.intersect - intersect).length();
        Ray l = Ray(intersect, (p.intersect - intersect).norm()),
            vB = Ray(p.intersect, (intersect - p.intersect).norm());
        if (scene->visible(l, p.object, dis)) {
            color += p.color * object->evaluate(v, l) * p.object->evaluate(p.lB, vB)
                / pow(dis, 2) * medium->evaluate(dis) / (traceDepth + 1 + rev);
        }
    }

    Ray l;
    Spectrum surfaceMul = object->sample(v, l);
    color = color + trace(idx, l, medium, traceDepth + 1, traceMul * mediumMul * surfaceMul) * surfaceMul;

    return color * mediumMul;
}


#endif
#endif //ARCRENDER_INCLUDE_TRACERS_BDPT_HPP
