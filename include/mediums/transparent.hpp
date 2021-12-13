#ifndef mediums_transparent_hpp
#define mediums_transparent_hpp

#include "../medium.hpp"

class Transparent : public Medium {
public:
    explicit Transparent(Spectrum color);

    Spectrum evaluate(double t) const override;
    shared_ptr<Shape> sample(double &t) const override;

    Spectrum color;
};

#ifdef ARC_IMPLEMENTATION

Transparent::Transparent(Spectrum color) : color(color) {}

Spectrum Transparent::evaluate(double t) const {
    return color ^ t;
}
shared_ptr<Shape> Transparent::sample(double &t) const {
    t = INF;
    return nullptr;
}

#endif
#endif //mediums_transparent_hpp
