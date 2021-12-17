#ifndef mediums_transparent_hpp
#define mediums_transparent_hpp

#include "../medium.hpp"

class Transparent : public Medium {
public:
    explicit Transparent(Spectrum spectrum);

    Spectrum evaluate(double t) const override;
    Spectrum sample(
        const shared_ptr<Scene> &scene, const Ray &v, shared_ptr<Shape> &object, Vec3 &intersect, Sampler &RNG
    ) const override;

private:
    Spectrum spectrum;
};

#ifdef ARC_IMPLEMENTATION

Transparent::Transparent(Spectrum spectrum) : spectrum(spectrum) {}

Spectrum Transparent::evaluate(double t) const {
    return (spectrum ^ t);
}

Spectrum Transparent::sample(
    const shared_ptr<Scene> &scene, const Ray &v, shared_ptr<Shape> &object, Vec3 &intersect, Sampler &RNG
) const {
    double t;
    scene->intersect(v, object, t);
    intersect = v.o + v.d * t;
    return spectrum ^ t;
}

#endif
#endif //mediums_transparent_hpp
