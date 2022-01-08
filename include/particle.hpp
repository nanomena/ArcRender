#ifndef particle_hpp
#define particle_hpp

#include "utils.hpp"
#include "vecmath.hpp"
#include "bxdf.hpp"
#include "light.hpp"
#include "medium.hpp"
#include "object.hpp"

class Particle : public Object {
public:
    explicit Particle(Spectrum color);

    bool isLight() const override;

    void sampleSurface(Vec3 &pos, Vec2 &texPos, double &pdf, Sampler &RNG) const override {
        throw invalid_argument("NotImplementedError");
    }
    double evaluateSurfaceImportance(const Vec3 &pos, const Vec2 &texPos) const override {
        throw invalid_argument("NotImplementedError");
    }

    Spectrum evaluateBxDF(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, const Vec3 &l) const override;
    Spectrum sampleBxDF(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, Vec3 &l, const Medium *&medium, Sampler &RNG) const override;
    double evaluateBxDFImportance(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, const Vec3 &l) const override;

    Spectrum evaluateLight(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos) const override;
    Spectrum evaluateLightBack(const Vec3 &lB, const Vec3 &pos, const Vec2 &texPos) const override;
    Spectrum sampleLight(Vec3 &lB, Vec3 &pos, Vec2 &texPos, const Medium *&medium, Sampler &RNG) const override;
    double evaluateLightImportance(const Vec3 &lB, const Vec3 &pos, const Vec2 &texPos) const override;

protected:
    Spectrum color;
};

#ifdef ARC_IMPLEMENTATION

Particle::Particle(Spectrum color) : color(color) {}

bool Particle::isLight() const {
    return false;
}

Spectrum Particle::evaluateBxDF(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, const Vec3 &l) const {
    assert(abs(1 - v.length()) < EPS);
    assert(abs(1 - l.length()) < EPS);
    Vec3 n = (-v + l).norm();
    return (color / (2 * pi)) * abs(l * n);
}

Spectrum Particle::sampleBxDF(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, Vec3 &l, const Medium *&medium, Sampler &RNG) const {
    assert(abs(1 - v.length()) < EPS);
    l = RNG.sphere();
    Vec3 n = (-v + l).norm();
    Spectrum s = (color / (2 * pi)) / (1 / (2 * pi));
    return s * abs(l * n);
}

double Particle::evaluateBxDFImportance(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, const Vec3 &l) const {
    Vec3 n = (-v + l).norm();
    return (1 / (2 * pi)) * abs(l * n);
}

Spectrum Particle::evaluateLight(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos) const {
    return Spectrum(0);
}

Spectrum Particle::evaluateLightBack(const Vec3 &lB, const Vec3 &pos, const Vec2 &texPos) const {
    return Spectrum(0);
}

Spectrum Particle::sampleLight(Vec3 &lB, Vec3 &pos, Vec2 &texPos, const Medium *&medium, Sampler &RNG) const {
    return Spectrum(0);
}

double Particle::evaluateLightImportance(const Vec3 &lB, const Vec3 &pos, const Vec2 &texPos) const {
    return 0;
}

#endif
#endif /* particle_hpp */