#ifndef Light_hpp
#define Light_hpp

#include "utils.hpp"
#include "sampler.hpp"
#include "spectrum.hpp"

class Light {
public:
    Spectrum evaluate(const Vec2 &texPos, const Vec3 &n, const Vec3 &gI) const;
    Spectrum sample(const Vec2 &texPos, const Vec3 &n, Vec3 &gI, Sampler &RNG) const;
    double evaluateImportance(const Vec2 &texPos, const Vec3 &n, const Vec3 &gI) const;

private:
    virtual Spectrum evaluate(const Vec2 &texPos, const Vec3 &i) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Spectrum sample(const Vec2 &texPos, Vec3 &i, double &pdf, Sampler &RNG) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual double evaluateImportance(const Vec2 &texPos, const Vec3 &i) const {
        throw invalid_argument("NotImplementedError");
    }
};

#ifdef ARC_IMPLEMENTATION

Spectrum Light::evaluate(const Vec2 &texPos, const Vec3 &n, const Vec3 &gI) const {
    Mat3 T, TInv;
    rotateAxis(n, gI, T, TInv);
    Vec3 vT = T * gI;
    return evaluate(texPos, vT);
}

Spectrum Light::sample(const Vec2 &texPos, const Vec3 &n, Vec3 &gI, Sampler &RNG) const {
    Mat3 T, TInv;
    rotateAxis(n, n, T, TInv);
    Vec3 vT;
    double pdf;
    Spectrum s = sample(texPos, vT, pdf, RNG);
    gI = TInv * vT;
    return s / pdf;
}

double Light::evaluateImportance(const Vec2 &texPos, const Vec3 &n, const Vec3 &gI) const {
    Mat3 T, TInv;
    rotateAxis(n, gI, T, TInv);
    Vec3 vT = T * gI;
    return evaluateImportance(texPos, vT);
}

#endif
#endif /* Light_hpp */
