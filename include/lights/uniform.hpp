#ifndef lights_uniform_hpp
#define lights_uniform_hpp

#include "../light.hpp"

class UniformLight : public Light {
public:
    explicit UniformLight(Spectrum color);
    Spectrum evaluate(const Vec3 &vLocal) const override;
    Spectrum sample(Vec3 &vLocal, double &pdf, Sampler &RNG) const override;
    double evaluateImportance(const Vec3 &vLocal) const;

private:
    Spectrum color;
};

#ifdef ARC_IMPLEMENTATION

UniformLight::UniformLight(Spectrum color) : color(color) {}

Spectrum UniformLight::evaluate(const Vec3 &vLocal) const {
    if (vLocal.z() < 0) return Spectrum(0); else return color;
}

Spectrum UniformLight::sample(Vec3 &vLocal, double &pdf, Sampler &RNG) const {
    vLocal = RNG.hemisphere();
    if (vLocal.z() < 0) assert(0);
    pdf = 1 / (2 * pi);
    return color;
}

double UniformLight::evaluateImportance(const Vec3 &vLocal) const {
    if (vLocal.z() < 0) return 0; else return 1 / (2 * pi);
}
#endif
#endif /* lights_uniform_hpp */
