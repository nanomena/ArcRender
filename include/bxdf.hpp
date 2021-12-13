#ifndef bxdf_hpp
#define bxdf_hpp

#include "utils.hpp"
#include "sampler.hpp"
#include "spectrum.hpp"

class BxDF {
public:
    Spectrum evaluateVtL(const Vec3 &n, const Vec3 &v, const Vec3 &l) const;
    Spectrum sampleVtL(const Vec3 &n, const Vec3 &v, Vec3 &l) const;
    Spectrum sampleLtV(const Vec3 &n, const Vec3 &l, Vec3 &v) const;
    void verifySample() const;

private:
    virtual Spectrum evaluateVtL(const Vec3 &vLocal, const Vec3 &lLocal) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Spectrum sampleVtL(const Vec3 &vLocal, Vec3 &lLocal, double &pdf) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Spectrum sampleLtV(const Vec3 &lLocal, Vec3 &vLocal, double &pdf) const {
        throw invalid_argument("NotImplementedError");
    }
};

#ifdef ARC_IMPLEMENTATION

void BxDF::verifySample() const {
    const int cnt = 1000000;
    for (int i = 0; i < 90; i += 10) {
        double w = i * pi / 180;
        Vec3 V(sin(w), 0, cos(w));
        double sum = 0;
        for (int j = 0; j < cnt; ++j) {
            Vec3 L;
            double pdf;
            sampleVtL(V, L, pdf);
            sum += 1 / pdf;
        }
        cout << i << ": " << sum / cnt << endl;
    }
}

Spectrum BxDF::evaluateVtL(const Vec3 &n, const Vec3 &v, const Vec3 &l) const {
    Mat3 T, TInv;
    rotateAxis(n, v, T, TInv);
    Vec3 vT = T * v, lT = T * l;
    return evaluateVtL(vT, lT);
}

Spectrum BxDF::sampleVtL(const Vec3 &n, const Vec3 &v, Vec3 &l) const {
    Mat3 T, TInv;
    rotateAxis(n, v, T, TInv);
    Vec3 vT = T * v, lT;
    double pdf;
    Spectrum s = sampleVtL(vT, lT, pdf);
    l = TInv * lT;
    return s / pdf;
}

Spectrum BxDF::sampleLtV(const Vec3 &n, const Vec3 &l, Vec3 &v) const {
    Mat3 T, TInv;
    rotateAxis(n, l, T, TInv);
    Vec3 lT = T * l, vT;
    double pdf;
    Spectrum s = sampleLtV(lT, vT, pdf);
    v = TInv * vT;
    return s / pdf;
}

#endif
#endif /* bxdf_hpp */
