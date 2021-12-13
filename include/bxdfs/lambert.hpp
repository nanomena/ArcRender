#ifndef bxdfs_lambert_hpp
#define bxdfs_lambert_hpp

#include "../bxdf.hpp"

class Lambert : public BxDF {
public:
    explicit Lambert(Spectrum color);

private:
    Spectrum evaluateVtL(const Vec3 &vLocal, const Vec3 &lLocal) const override;
    Spectrum sampleVtL(const Vec3 &vLocal, Vec3 &lLocal, double &pdf) const override;
    Spectrum sampleLtV(const Vec3 &lLocal, Vec3 &vLocal, double &pdf) const override;

    Spectrum color;
};

#ifdef ARC_IMPLEMENTATION

Lambert::Lambert(Spectrum color) : color(color) {}

Spectrum Lambert::evaluateVtL(const Vec3 &V, const Vec3 &L) const {
    if (L[2] * V[2] < 0) return Spectrum(0); else return color / pi;
}

Spectrum Lambert::sampleVtL(const Vec3 &vLocal, Vec3 &lLocal, double &pdf) const {
    lLocal = RD.hemisphere();
    if (vLocal[2] < 0) lLocal[2] *= -1;
    pdf = 1 / (2 * pi);
    return color / pi;
}

Spectrum Lambert::sampleLtV(const Vec3 &lLocal, Vec3 &vLocal, double &pdf) const {
    vLocal = RD.hemisphere();
    if (lLocal[2] < 0) vLocal[2] *= -1;
    pdf = 1 / (2 * pi);
    return color / pi;
}

#endif
#endif /* bxdfs_lambert_hpp */
