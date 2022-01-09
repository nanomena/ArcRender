#ifndef lights_uniform_hpp
#define lights_uniform_hpp

#include "../light.hpp"

class UniformLight : public Light {
public:
    explicit UniformLight(Spectrum color, bool sphere = false);
    Spectrum evaluate(const Vec2 &texPos, const Vec3 &i) const override;
    Spectrum sample(const Vec2 &texPos, Vec3 &i, double &pdf, Sampler &RNG) const override;
    double evaluateImportance(const Vec2 &texPos, const Vec3 &i) const override;

private:
    bool sphere;
    Spectrum color;
};

#ifdef ARC_IMPLEMENTATION

UniformLight::UniformLight(Spectrum color, bool sphere) : color(color), sphere(sphere) {}

Spectrum UniformLight::evaluate(const Vec2 &texPos, const Vec3 &i) const {
    if (sphere) return color;
    if (i.z() < 0) return Spectrum(0); else return color;
}

Spectrum UniformLight::sample(const Vec2 &texPos, Vec3 &i, double &pdf, Sampler &RNG) const {
    if (sphere) {
        i = RNG.sphere();
        pdf = 1 / (4 * pi);
    } else {
        i = RNG.hemisphere();
        pdf = 1 / (2 * pi);
    }
    return color;
}

double UniformLight::evaluateImportance(const Vec2 &texPos, const Vec3 &i) const {
    if (sphere) return 1 / (4 * pi);
    if (i.z() < 0) return 0; else return 1 / (2 * pi);
}
#endif
#endif /* lights_uniform_hpp */
