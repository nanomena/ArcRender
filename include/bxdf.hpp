#ifndef bxdf_hpp
#define bxdf_hpp

#include "utils.hpp"
#include "sampler.hpp"
#include "spectrum.hpp"

class BxDF {
public:
    Spectrum evaluate(const Vec3 &n, const Vec3 &v, const Vec3 &l) const;
    Spectrum sample(const Vec3 &n, const Vec3 &v, Vec3 &l, Sampler &RNG) const;
    virtual bool glossy() const {
        throw invalid_argument("NotImplementedError");
    }

private:
    virtual Spectrum evaluate(const Vec3 &vLocal, const Vec3 &lLocal) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Spectrum sample(const Vec3 &vLocal, Vec3 &lLocal, double &pdf, Sampler &RNG) const {
        throw invalid_argument("NotImplementedError");
    }
};

#ifdef ARC_IMPLEMENTATION

Spectrum BxDF::evaluate(const Vec3 &n, const Vec3 &v, const Vec3 &l) const {
    Mat3 T, TInv;
    rotateAxis(n, v, T, TInv);
    Vec3 vT = T * v, lT = T * l;
    return evaluate(vT, lT);
}

Spectrum BxDF::sample(const Vec3 &n, const Vec3 &v, Vec3 &l, Sampler &RNG) const {
    Mat3 T, TInv;
    rotateAxis(n, v, T, TInv);
    Vec3 vT = T * v, lT;
    double pdf;
    Spectrum s = sample(vT, lT, pdf, RNG);
    l = TInv * lT;
    return s / pdf;
}

#endif
#endif /* bxdf_hpp */
