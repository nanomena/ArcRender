#ifndef mediums_transparent_hpp
#define mediums_transparent_hpp

#include "../medium.hpp"

class Transparent : public Medium {
public:
    explicit Transparent(Spectrum spectrum);

    Spectrum evaluate(
        const shared_ptr<Scene> &scene, const Ray &v, const shared_ptr<Shape> &object, const Vec3 &pos,
        const shared_ptr<Shape> &tObject, const Vec3 &target, const function<Spectrum(const Vec3 &)> &link, Sampler &RNG
    ) const override;
    Spectrum sample(
        const shared_ptr<Scene> &scene, const Ray &v, Ray &l, shared_ptr<Shape> &object, Vec3 &intersect, Sampler &RNG
    ) const override;

private:
    Spectrum spectrum;
};

#ifdef ARC_IMPLEMENTATION

Transparent::Transparent(Spectrum spectrum) : spectrum(spectrum) {}

Spectrum Transparent::evaluate(
    const shared_ptr<Scene> &scene, const Ray &v, const shared_ptr<Shape> &object, const Vec3 &pos,
    const shared_ptr<Shape> &tObject, const Vec3 &target, const function<Spectrum(const Vec3 &)> &link, Sampler &RNG
) const {
    shared_ptr<Shape> actual;
    double t;
    Ray l = {pos, (target - pos).norm()};
    scene->intersect(l, actual, t);
    if ((abs((target - pos).length() - t) < EPS) && (actual == tObject)
        && (object->getMedium(l).get() == this)
        && (tObject->getMedium({target, -l.d}).get() == this))
        return object->evaluateBxDF(v, l) * link(pos) / pow(t, 2) * (spectrum ^ t);
    return Spectrum(0);
}

Spectrum Transparent::sample(
    const shared_ptr<Scene> &scene, const Ray &v, Ray &l, shared_ptr<Shape> &object, Vec3 &intersect, Sampler &RNG
) const {
    double t;
    scene->intersect(v, object, t);
    intersect = v.o + v.d * t;
    l = v;
    return spectrum ^ t;
}

#endif
#endif //mediums_transparent_hpp
