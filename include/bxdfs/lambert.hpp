#ifndef bxdfs_lambert_hpp
#define bxdfs_lambert_hpp

#include "../bxdf.hpp"

class Lambert : public BxDF {
public:
    explicit Lambert(Spectrum color);

private:
    Spectrum evaluate(const Vec3 &vLocal, const Vec3 &lLocal) const override;
    Spectrum sample(const Vec3 &vLocal, Vec3 &lLocal, double &pdf, Sampler &RNG) const override;
    double evaluateImportance(const Vec3 &vLocal, const Vec3 &lLocal) const override;

    Spectrum color;
};

#ifdef ARC_IMPLEMENTATION

Lambert::Lambert(Spectrum color) : color(color) {}

Spectrum Lambert::evaluate(const Vec3 &vLocal, const Vec3 &lLocal) const {
    if (vLocal[2] * lLocal[2] < 0) return Spectrum(0); else return color / pi;
}

Spectrum Lambert::sample(const Vec3 &vLocal, Vec3 &lLocal, double &pdf, Sampler &RNG) const {
    lLocal = RNG.hemisphere();
    if (vLocal[2] < 0) lLocal[2] *= -1;
    pdf = 1 / (2 * pi);
    return color / pi;
}

double Lambert::evaluateImportance(const Vec3 &vLocal, const Vec3 &lLocal) const {
    if (vLocal[2] * lLocal[2] < 0) return 0; else return 1 / (2 * pi);
}

#endif
#endif /* bxdfs_lambert_hpp */
