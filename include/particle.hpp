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

    void sampleSurface(Vec3 &pos, double &pdf, Sampler &RNG) const override {
        throw invalid_argument("NotImplementedError");
    }
    double evaluateSurfaceImportance(const Vec3 &pos) const override {
        throw invalid_argument("NotImplementedError");
    }

    Spectrum evaluateBxDF(const Ray &v, const Ray &l) const override;
    Spectrum sampleBxDF(const Ray &v, const Vec3 &pos, Ray &l, const Medium *&medium, Sampler &RNG) const override;
    double evaluateBxDFImportance(const Ray &v, const Ray &l) const override;

    Spectrum evaluateLight(const Ray &v, const Vec3 &pos) const override;
    Spectrum evaluateLightBack(const Ray &vB) const override;
    Spectrum sampleLight(Ray &lB, const Medium *&medium, Sampler &RNG) const override;
    double evaluateLightImportance(const Ray &lB) const override;

protected:
    Spectrum color;
};

#ifdef ARC_IMPLEMENTATION

Particle::Particle(Spectrum color) : color(color) {}

bool Particle::isLight() const {
    return false;
}

Spectrum Particle::evaluateBxDF(const Ray &v, const Ray &l) const {
    assert(abs(1 - v.d.length()) < EPS);
    assert(abs(1 - l.d.length()) < EPS);
    Vec3 n = (-v.d + l.d).norm();
    return (color / (2 * pi)) * abs(l.d * n);
}

double Particle::evaluateBxDFImportance(const Ray &v, const Ray &l) const {
    Vec3 n = (-v.d + l.d).norm();
    return (1 / (2 * pi)) * abs(l.d * n);
}

Spectrum Particle::sampleBxDF(const Ray &v, const Vec3 &pos, Ray &l, const Medium *&medium, Sampler &RNG) const {
    assert(abs(1 - v.d.length()) < EPS);
    l.o = pos;
    l.d = RNG.sphere();
    Vec3 n = (-v.d + l.d).norm();
    Spectrum s = (color / (2 * pi)) / (1 / (2 * pi));
    return s * abs(l.d * n);
}

Spectrum Particle::evaluateLight(const Ray &v, const Vec3 &pos) const {
    return Spectrum(0);
}

Spectrum Particle::evaluateLightBack(const Ray &vB) const {
    return Spectrum(0);
}

Spectrum Particle::sampleLight(Ray &lB, const Medium *&medium, Sampler &RNG) const {
    return Spectrum(0);
}

double Particle::evaluateLightImportance(const Ray &lB) const {
    return 0;
}

#endif
#endif /* particle_hpp */