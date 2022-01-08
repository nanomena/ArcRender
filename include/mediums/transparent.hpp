#ifndef mediums_transparent_hpp
#define mediums_transparent_hpp

#include "../medium.hpp"

class Transparent : public Medium {
public:
    explicit Transparent(Spectrum spectrum);

    Spectrum evaluate(double t) const override;
    Spectrum sample(
        const Scene *scene, const Ray &v, const Object *&object, Vec3 &pos, Vec2 &texPos, Sampler &RNG
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
    const Scene *scene, const Ray &v, const Object *&object, Vec3 &pos, Vec2 &texPos, Sampler &RNG
) const {
    double t;
    const Shape *shape;
    scene->intersect(v, shape, t, pos, texPos);
    object = shape;
    return spectrum ^ t;
}

#endif
#endif //mediums_transparent_hpp
