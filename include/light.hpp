#ifndef Light_hpp
#define Light_hpp

#include "utils.hpp"
#include "sampler.hpp"
#include "spectrum.hpp"

class Light {
public:
    Spectrum evaluate(const Vec3 &n, const Vec3 &v) const;
    Spectrum sample(const Vec3 &n, Vec3 &v, Sampler &RNG) const;
    double evaluateImportance(const Vec3 &n, const Vec3 &v) const;

private:
    virtual Spectrum evaluate(const Vec3 &vLocal) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Spectrum sample(Vec3 &vLocal, double &pdf, Sampler &RNG) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual double evaluateImportance(const Vec3 &vLocal) const {
        throw invalid_argument("NotImplementedError");
    }
};

#ifdef ARC_IMPLEMENTATION

Spectrum Light::evaluate(const Vec3 &n, const Vec3 &v) const {
    Mat3 T, TInv;
    rotateAxis(n, v, T, TInv);
    Vec3 vT = T * v;
    return evaluate(vT);
}

Spectrum Light::sample(const Vec3 &n, Vec3 &v, Sampler &RNG) const {
    Mat3 T, TInv;
    rotateAxis(n, n, T, TInv);
    Vec3 vT;
    double pdf;
    Spectrum s = sample(vT, pdf, RNG);
    v = TInv * vT;
    return s / pdf;
}

double Light::evaluateImportance(const Vec3 &n, const Vec3 &v) const {
    Mat3 T, TInv;
    rotateAxis(n, v, T, TInv);
    Vec3 vT = T * v;
    return evaluateImportance(vT);
}

#endif
#endif /* Light_hpp */
