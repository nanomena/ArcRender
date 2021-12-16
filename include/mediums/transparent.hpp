#ifndef mediums_transparent_hpp
#define mediums_transparent_hpp

#include "../medium.hpp"

class Transparent : public Medium {
public:
    explicit Transparent(Spectrum color);

    Spectrum evaluate(double t) const override;
    Spectrum sample(double &t, shared_ptr<Shape> &object, Sampler &RNG) const override;

    Spectrum color;
};

#ifdef ARC_IMPLEMENTATION

Transparent::Transparent(Spectrum color) : color(color) {}

Spectrum Transparent::evaluate(double t) const {
    return color ^ t;
}
Spectrum Transparent::sample(double &t, shared_ptr<Shape> &object, Sampler &RNG) const {
    return color ^ t;
}

#endif
#endif //mediums_transparent_hpp
