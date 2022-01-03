#ifndef lights_uniform_hpp
#define lights_uniform_hpp

#include "../light.hpp"

class UniformLight : public Light {
public:
    explicit UniformLight(Spectrum color);
    Spectrum evaluate(const Vec3 &pos, const Vec3 &i) const override;
    Spectrum sample(const Vec3 &pos, Vec3 &i, double &pdf, Sampler &RNG) const override;
    double evaluateImportance(const Vec3 &pos, const Vec3 &i) const override;

private:
    Spectrum color;
};

#ifdef ARC_IMPLEMENTATION

UniformLight::UniformLight(Spectrum color) : color(color) {}

Spectrum UniformLight::evaluate(const Vec3 &pos, const Vec3 &i) const {
    if (i.z() < 0) return Spectrum(0); else return color;
}

Spectrum UniformLight::sample(const Vec3 &pos, Vec3 &i, double &pdf, Sampler &RNG) const {
    i = RNG.hemisphere();
    if (i.z() < 0) assert(0);
    pdf = 1 / (2 * pi);
    return color;
}

double UniformLight::evaluateImportance(const Vec3 &pos, const Vec3 &i) const {
    if (i.z() < 0) return 0; else return 1 / (2 * pi);
}
#endif
#endif /* lights_uniform_hpp */
