#ifndef mediums_transparent_hpp
#define mediums_transparent_hpp

#include "../medium.hpp"

class Transparent : public Medium {
public:
    explicit Transparent(Spectrum color);

    Spectrum evaluate(
        const shared_ptr<Scene> &scene, const Ray &v, const shared_ptr<Shape> &object, const Vec3 &pos,
        const shared_ptr<Shape> &targetO, const Vec3 &target, const function<Spectrum(const Vec3 &)> &link, Sampler &RNG
    ) const override;
    Spectrum sample(
        const shared_ptr<Scene> &scene, const Ray &v, shared_ptr<Shape> &object, Vec3 &intersect, Sampler &RNG
    ) const override;

    Spectrum color;
};

#ifdef ARC_IMPLEMENTATION

Transparent::Transparent(Spectrum color) : color(color) { identifier = "transparent"; }

Spectrum Transparent::evaluate(
    const shared_ptr<Scene> &scene, const Ray &v, const shared_ptr<Shape> &object, const Vec3 &pos,
    const shared_ptr<Shape> &targetO, const Vec3 &target, const function<Spectrum(const Vec3 &)> &link, Sampler &RNG
) const {
    double t;
    Ray l = {pos, (target - pos).norm()};
    shared_ptr<Shape> actual;
    scene->intersect(l, actual, t);
    if (abs((target - pos).length() - t) > EPS) return Spectrum(0);
    if (actual != targetO) return Spectrum(0);

    if (object->getMedium({pos, -l.d}) != object->getMedium({l.o, l.d})) return Spectrum(0);
//    cerr << "visible" << endl;
    return object->evaluateBxDF(v, l) * link(l.o) / pow(t, 2) * (color ^ t);
}

Spectrum Transparent::sample(
    const shared_ptr<Scene> &scene, const Ray &v, shared_ptr<Shape> &object, Vec3 &intersect, Sampler &RNG
) const {
    double t;
    scene->intersect(v, object, t);
    return color ^ t;
}

#endif
#endif //mediums_transparent_hpp
