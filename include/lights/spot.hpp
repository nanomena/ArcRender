#ifndef lights_spot_hpp
#define lights_spot_hpp

#include "../light.hpp"

class SpotLight : public Light {
public:
    explicit SpotLight(Spectrum color, double alpha);
    Spectrum evaluate(const Vec2 &texPos, const Vec3 &i) const override;
    Spectrum sample(const Vec2 &texPos, Vec3 &i, double &pdf, Sampler &RNG) const override;
    double evaluateImportance(const Vec2 &texPos, const Vec3 &i) const override;

    double D(const Vec3 &n) const;

private:
    double alpha;
    Spectrum color;
};

#ifdef ARC_IMPLEMENTATION

SpotLight::SpotLight(Spectrum color, double alpha) : color(color), alpha(alpha) {}

double SpotLight::D(const Vec3 &n) const {
    double alpha2 = alpha * alpha;
    return alpha2 / (pi * pow(n.z() * n.z() * (alpha2 - 1) + 1, 2));
}

Spectrum SpotLight::evaluate(const Vec2 &texPos, const Vec3 &i) const {
    if (i.z() < 0) return Spectrum(0); else return color * D(i);
}

Spectrum SpotLight::sample(const Vec2 &texPos, Vec3 &i, double &pdf, Sampler &RNG) const {
    double alpha2 = alpha * alpha;
    double p = RNG.rand(), q = RNG.rand(-pi, pi);
    double cos_n = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_n = sqrt(1 - cos_n * cos_n);
    i = Vec3(cos(q) * sin_n, sin(q) * sin_n, cos_n);
    pdf = alpha2 * cos_n / (pi * pow((alpha2 - 1) * cos_n * cos_n + 1, 2));
    return color;
}

double SpotLight::evaluateImportance(const Vec2 &texPos, const Vec3 &i) const {
    if (i.z() < 0) return 0; else return D(i);
}
#endif
#endif /* lights_spot_hpp */
