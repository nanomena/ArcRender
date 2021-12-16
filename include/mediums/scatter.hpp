#ifndef mediums_scatter_hpp
#define mediums_scatter_hpp

#include "../medium.hpp"

class Scatter : public Medium {
public:
    explicit Scatter(double lambda, Spectrum color, int maxTrace);

    Spectrum traceEvaluate(
        const shared_ptr<Scene> &scene, const Ray &v, const shared_ptr<Shape> &tObject, const Vec3 &target,
        const function<Spectrum(const Vec3 &)> &link, int traceDepth, Sampler &RNG
    ) const;

    Spectrum evaluate(
        const shared_ptr<Scene> &scene, const Ray &v, const shared_ptr<Shape> &object, const Vec3 &pos,
        const shared_ptr<Shape> &tObject, const Vec3 &target, const function<Spectrum(const Vec3 &)> &link, Sampler &RNG
    ) const override;

    Spectrum traceSample(
        const shared_ptr<Scene> &scene, const Ray &v, Ray &f, shared_ptr<Shape> &object, Vec3 &intersect,
        int traceDepth, Sampler &RNG
    ) const;

    Spectrum sample(
        const shared_ptr<Scene> &scene, const Ray &v, Ray &f, shared_ptr<Shape> &object, Vec3 &intersect, Sampler &RNG
    ) const override;

private:
    double lambda;
    Spectrum color;
    int maxTrace;
};

#ifdef ARC_IMPLEMENTATION

Scatter::Scatter(double lambda, Spectrum color, int maxTrace) : lambda(lambda), color(color), maxTrace(maxTrace) {}

Spectrum Scatter::traceEvaluate(
    const shared_ptr<Scene> &scene, const Ray &v, const shared_ptr<Shape> &tObject, const Vec3 &target,
    const function<Spectrum(const Vec3 &)> &link, int traceDepth, Sampler &RNG
) const {
    double t;
    shared_ptr<Shape> actual;
    scene->intersect(v, actual, t);
    double tCur = log(1 - RNG.rand()) / (-lambda);
    if (tCur > t || traceDepth > maxTrace) {
        return Spectrum(0);
    } else {
        Spectrum spectrum(0);
        Vec3 pos = v.o + v.d * tCur;
        Ray l = {pos, (target - pos).norm()};
        Vec3 n = (-v.d + l.d).norm();

        scene->intersect(l, actual, t);
        if ((abs((target - pos).length() - t) < EPS) && (actual == tObject))
            spectrum += color * (n * l.d) * link(pos) / pow(t, 2) * exp(-lambda * t);

        l = {pos, RNG.sphere()};
        n = (-v.d + l.d).norm();
            spectrum += color * (n * l.d) * traceEvaluate(scene, l, tObject, target, link, traceDepth + 1, RNG);
        return spectrum;
    }
}

Spectrum Scatter::evaluate(
    const shared_ptr<Scene> &scene, const Ray &v, const shared_ptr<Shape> &object, const Vec3 &pos,
    const shared_ptr<Shape> &tObject, const Vec3 &target, const function<Spectrum(const Vec3 &)> &link, Sampler &RNG
) const {
    Spectrum spectrum;
    shared_ptr<Shape> actual;
    double t;
    Ray l = {pos, (target - pos).norm()};
    scene->intersect(l, actual, t);
    if ((abs((target - pos).length() - t) < EPS) && (actual == tObject)
        && (object->getMedium(l).get() == this)
        && (tObject->getMedium({target, -l.d}).get() == this))
        spectrum = object->evaluateBxDF(v, l) * link(pos) / pow(t, 2) * exp(-lambda * t);

    shared_ptr<Medium> actualMedium;
    Spectrum mul = object->sampleBxDF(v, l, actualMedium, RNG);
    if (actualMedium.get() == this)
        spectrum += mul * traceEvaluate(scene, l, tObject, target, link, 1, RNG);
    return spectrum;
}

Spectrum Scatter::traceSample(
    const shared_ptr<Scene> &scene, const Ray &v, Ray &f, shared_ptr<Shape> &object, Vec3 &intersect, int traceDepth,
    Sampler &RNG
) const {
    double t;
    scene->intersect(v, object, t);
    double tCur = log(1 - RNG.rand()) / (-lambda);
    if (tCur > t || traceDepth > maxTrace) {
        intersect = v.o + v.d * t;
        f = v;
        return Spectrum(1);
    } else {
        Vec3 pos = v.o + v.d * tCur;
        Ray l = {pos, RNG.sphere()};
        Vec3 n = (-v.d + l.d).norm();
        return color * (n * l.d) * traceSample(scene, l, f, object, intersect, traceDepth + 1, RNG);
    }
}

Spectrum Scatter::sample(
    const shared_ptr<Scene> &scene, const Ray &v, Ray &f, shared_ptr<Shape> &object, Vec3 &intersect, Sampler &RNG
) const {
    return traceSample(scene, v, f, object, intersect, 1, RNG);
}

#endif
#endif //mediums_scatter_hpp
