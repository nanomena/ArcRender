#ifndef ARCRENDER_INCLUDE_TRACERS_BDPT_HPP
#define ARCRENDER_INCLUDE_TRACERS_BDPT_HPP

#include "../tracer.hpp"

struct Hit {
    Vec3 intersect;
    Ray lB;
    shared_ptr<Shape> object;
    Spectrum color;
};

class BidirectionalPathTracer : public Tracer {
public:
    void initCache() override;
    void preSample(int idx) override;
    void sample(int idx) override;

    BidirectionalPathTracer(
        int width, int height, const shared_ptr<Scene> &scene, int traceLimit = 6, double traceEps = 1e-4
    );

private:
    vector<vector<Hit>> photons;
    int traceLimit;
    double traceEPS;
};

#ifdef ARC_IMPLEMENTATION

BidirectionalPathTracer::BidirectionalPathTracer(
    int width, int height, const shared_ptr<Scene> &scene, int traceLimit, double traceEPS
) : Tracer(width, height, scene, traceLimit, traceLimit), traceLimit(traceLimit), traceEPS(traceEPS) {}

void BidirectionalPathTracer::initCache() {
    photons.clear();
    photons.resize(traceLimit - 1);
}

void BidirectionalPathTracer::preSample(int idx) {
    Ray lB;
    Spectrum mul = scene->lights[idx % scene->lights.size()]->sampleStV(lB);
    for (int cnt = 1; cnt + 1 < traceLimit; ++cnt) {

//        cerr << "B " << cnt << " " << lB.o << " " << lB.d << endl;

        shared_ptr<Shape> object;
        double t;
        scene->intersect(lB, object, t);

//        cerr << object->name << endl;

        if (object == scene->skybox) break;

        Vec3 intersect = lB.o + t * lB.d;
        photons[cnt].push_back(Hit{intersect, lB, object, mul});

        Ray vB;
        mul = mul * object->sampleLtV(lB, vB);
        if (mul.norm() < traceEPS) break;
        lB = vB;
    }
}

void BidirectionalPathTracer::sample(int idx) {
    Ray v = sampleCamera(idx);
    Spectrum mul(1);
    for (int cnt = 0; cnt < traceLimit; ++cnt) {

//        cerr << "F " << cnt << " " << v.o << " " << v.d << " " << mul << endl;

        shared_ptr<Shape> object;
        double t;
        scene->intersect(v, object, t);

//        cerr << object->name << endl;

        Vec3 intersect = v.o + t * v.d;
        if (object->isLight()) {
            Spectrum color = object->evaluateVtS(v) * mul;
            add(idx, color, cnt, cnt);
        } else {
            add(idx, Spectrum(0), cnt, cnt);
        }

        if (object == scene->skybox) break;

        if (cnt + 1 < traceLimit)
            for (const auto &light: scene->lights) {
                Ray l;
                double dis;
                Spectrum color = light->sampleS(intersect, l, dis) * mul;
                if (scene->visible(l, light, dis)) {
                    color = color * object->evaluateVtL(v, l) / pow(dis, 2);
                    add(idx, color, cnt + 1, cnt);
                }
            }
        for (int rev = 1; cnt + rev + 1 < traceLimit; ++rev)
            for (const auto &p: photons[rev]) {
                double dis = (p.intersect - intersect).length();
                Ray l = Ray(intersect, (p.intersect - intersect).norm()),
                    vB = Ray(p.intersect, (intersect - p.intersect).norm());
                Spectrum color = p.color * mul;
                if (scene->visible(l, p.object, dis)) {
                    color = color * object->evaluateVtL(v, l) * p.object->evaluateLtV(p.lB, vB) / pow(dis, 2);
                    add(idx, color, cnt + rev + 1, cnt);
                }
            }

        Ray l;
        mul = mul * object->sampleVtL(v, l);
        if (mul.norm() < traceEPS) break;
        v = l;
    }
}

#endif
#endif //ARCRENDER_INCLUDE_TRACERS_BDPT_HPP
