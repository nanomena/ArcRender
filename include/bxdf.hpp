#ifndef bxdf_hpp
#define bxdf_hpp

#include "utils.hpp"
#include "sampler.hpp"
#include "spectrum.hpp"

class BxDF {
public:
    Spectrum evaluate(const Vec2 &texPos, const Vec3 &n, const Vec3 &t, const Vec3 &gI, const Vec3 &gO) const;
    Spectrum sample(const Vec2 &texPos, const Vec3 &n, const Vec3 &t, const Vec3 &gI, Vec3 &gO, Sampler &RNG) const;
    double evaluateImportance(const Vec2 &texPos, const Vec3 &n, const Vec3 &t, const Vec3 &gI, const Vec3 &gO) const;

private:
    virtual Spectrum evaluate(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Spectrum sample(const Vec2 &texPos, const Vec3 &i, Vec3 &o, double &pdf, Sampler &RNG) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual double evaluateImportance(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const {
        throw invalid_argument("NotImplementedError");
    }
};

#ifdef ARC_IMPLEMENTATION

Spectrum BxDF::evaluate(const Vec2 &texPos, const Vec3 &n, const Vec3 &t, const Vec3 &gI, const Vec3 &gO) const {
    Mat3 T, TInv;
    rotateAxis(n, t, T, TInv);
    Vec3 vT = T * gI, lT = T * gO;
    return evaluate(texPos, vT, lT);
}

Spectrum BxDF::sample(const Vec2 &texPos, const Vec3 &n, const Vec3 &t, const Vec3 &gI, Vec3 &gO, Sampler &RNG) const {
    Mat3 T, TInv;
    rotateAxis(n, t, T, TInv);
    Vec3 vT = T * gI, lT;
    double pdf;
    Spectrum s = sample(texPos, vT, lT, pdf, RNG);
    gO = TInv * lT;
    return s / pdf;
}

double BxDF::evaluateImportance(const Vec2 &texPos, const Vec3 &n, const Vec3 &t, const Vec3 &gI, const Vec3 &gO) const {
    Mat3 T, TInv;
    rotateAxis(n, t, T, TInv);
    Vec3 vT = T * gI, lT = T * gO;
    return evaluateImportance(texPos, vT, lT);
}

#endif
#endif /* bxdf_hpp */
