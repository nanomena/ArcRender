#ifndef bxdfs_lambert_hpp
#define bxdfs_lambert_hpp

#include "../bxdf.hpp"

class Lambert : public BxDF {

public:
    Lambert() = default;
    void evaluate(const Vec3 &V, const Vec3 &L, double &weight) override;
    void sample_VtL(const Vec3 &V, Vec3 &L, double &pdf) override;
    void sample_LtV(const Vec3 &L, Vec3 &V, double &pdf) override;
};

#ifdef ARC_IMPLEMENTATION

void Lambert::evaluate(const Vec3 &V, const Vec3 &L, double &weight) {
    if (L[2] * V[2] < 0) return (weight = 0, void());
    weight = 1. / pi;
}

void Lambert::sample_VtL(const Vec3 &V, Vec3 &L, double &pdf) {
    L = RD.semisphere();
    if (V[2] < 0) L[2] *= -1;
    pdf = 1 / (2 * pi);
}

void Lambert::sample_LtV(const Vec3 &L, Vec3 &V, double &pdf) {
    V = RD.semisphere();
    if (L[2] < 0) V[2] *= -1;
    pdf = 1 / (2 * pi);
}

#endif
#endif /* bxdfs_lambert_hpp */
