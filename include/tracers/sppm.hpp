#ifndef ARCRENDER_INCLUDE_TRACERS_SPPM_HPP
#define ARCRENDER_INCLUDE_TRACERS_SPPM_HPP

#include "../tracer.hpp"
#include "../tree.hpp"

class StochasticProgressivePhotonMapping : public Tracer {
public:
    void initGraph(int preCnt) override;
    void buildGraph() override;
    void preSample(int idx) override;
    void sample(int idx) override;

    void revTrace(const int &idx, const Ray &lB, const shared_ptr<Medium> &medium, int traceDepth, Spectrum traceMul);
    Spectrum trace(const int &idx, const Ray &v, const shared_ptr<Medium> &medium, int traceDepth, Spectrum traceMul);

    StochasticProgressivePhotonMapping(
        int width, int height, const shared_ptr<Scene> &scene, int traceLimit = 8, double traceEps = 1e-7
    );

private:
    vector<double> squaredRadius, nPhotons;
    vector<Spectrum> graphSpectrum;
    int totPhoton;

    vector<shared_ptr<Photon>> photons;
    shared_ptr<KaTenTree> tree;
    int traceLimit;
    double traceEPS;
};

#ifdef ARC_IMPLEMENTATION

StochasticProgressivePhotonMapping::StochasticProgressivePhotonMapping(
    int width, int height, const shared_ptr<Scene> &scene, int traceLimit, double traceEPS
) : Tracer(width, height, scene, traceLimit, traceLimit), traceLimit(traceLimit), traceEPS(traceEPS) {
    squaredRadius.resize(width * height);
    nPhotons.resize(width * height);
    graphSpectrum.resize(width * height);
    Box3 b = scene->box();
    double radius = ((b.U() - b.L()).x() + (b.U() - b.L()).y() + (b.U() - b.L()).z()) / 3 / ((width + height) / 2.);

    for (int i = 0; i < width * height; ++i)
        squaredRadius[i] = radius * radius;
}

void StochasticProgressivePhotonMapping::initGraph(int preCnt) {
    totPhoton = preCnt;
    photons.clear();

    double maxRadius = 0, avgRadius = 0;
    for (int i = 0; i < width * height; ++i) {
        maxRadius = max(maxRadius, sqrt(squaredRadius[i]));
        avgRadius += sqrt(squaredRadius[i]) / (width * height);
    }
    cerr << "Average radius: " << avgRadius << " Max Radius: " << maxRadius << endl;
}

void StochasticProgressivePhotonMapping::buildGraph() {
    tree = make_shared<KaTenTree>(KaTenTree(photons));
}

void StochasticProgressivePhotonMapping::preSample(int idx) {
    Ray lB;
    Spectrum traceMul = scene->lights[idx % scene->lights.size()]->sample(lB);
    revTrace(idx, lB, scene->medium, 1, traceMul);
}

void StochasticProgressivePhotonMapping::sample(int idx) {
    Ray v = sampleCamera(idx);
    C[idx] -= graphSpectrum[idx] / (pi * squaredRadius[idx]);
    C[idx] += trace(idx, v, scene->medium, 1, Spectrum(1));

//    cerr << C[idx] << " " << graphSpectrum[idx] << endl;

    C[idx] += graphSpectrum[idx] / (pi * squaredRadius[idx]);
    W[idx] += 1;
}

void StochasticProgressivePhotonMapping::revTrace(
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
        revTrace(idx, vB, medium, max(2, traceDepth), traceMul * mediumMul * surfaceMul);
        return;
    }
//    if (traceDepth > 1) {
    photons.push_back(make_shared<Photon>(Photon{intersect, lB, object, traceMul * mediumMul}));
//    }

    Ray vB;
    Spectrum surfaceMul = object->sample(lB, vB);
    revTrace(idx, vB, medium, traceDepth + 1, traceMul * mediumMul * surfaceMul);
}

Spectrum StochasticProgressivePhotonMapping::trace(
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
    Spectrum color = object->evaluate(v);

    if (object->glossy(intersect)) {
        Ray l;
        Spectrum surfaceMul = object->sample(v, l);
        color = color + trace(idx, l, medium, traceDepth, traceMul * mediumMul * surfaceMul) * surfaceMul;

        return color * mediumMul;
    }

//    for (const auto &light: scene->lights) {
//        Ray l;
//        double dis;
//
//        Spectrum baseColor = light->sample(intersect, l, dis);
//        if (scene->visible(l, light, dis)) {
//            color += baseColor * object->evaluate(v, l) / pow(dis, 2) * medium->evaluate(dis);
//        }
//    }

    Spectrum photonSum;
    int mPhotons = 0;
    tree->search(
        intersect, sqrt(squaredRadius[idx]), [&](const shared_ptr<Photon> &cen) {
            Ray l = Ray(cen->intersect, -cen->lB.d);
            Spectrum baseColor = cen->color * cen->object->evaluate(v, l);
            photonSum += baseColor;
            mPhotons += 1;
        }
    );

//    cerr << idx << ": " << mPhotons << " / " << totPhoton << " | " << photonSum << endl;
    if (mPhotons > 0) {
        double alpha = 2. / 3;
        double ration = (nPhotons[idx] + alpha * mPhotons) / (nPhotons[idx] + mPhotons);
        squaredRadius[idx] *= ration;
        graphSpectrum[idx] = (graphSpectrum[idx] + photonSum * mediumMul / totPhoton) * ration;
        nPhotons[idx] += alpha * mPhotons;
    }

    return color * mediumMul;
}

#endif
#endif //ARCRENDER_INCLUDE_TRACERS_SPPM_HPP
