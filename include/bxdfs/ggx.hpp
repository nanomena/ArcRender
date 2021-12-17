#ifndef bxdfs_ggx_hpp
#define bxdfs_ggx_hpp

#include "../bxdf.hpp"

class GGX : public BxDF {
public:
    GGX(Spectrum F0, double rough);
    bool glossy() const override {
        return rough < 0.2;
    }

private:
    Spectrum evaluate(const Vec3 &vLocal, const Vec3 &lLocal) const override;
    Spectrum sample(const Vec3 &vLocal, Vec3 &lLocal, double &pdf, Sampler &RNG) const override;
    double evaluatePdf(const Vec3 &vLocal, const Vec3 &lLocal) const override;

    Spectrum F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;
    double D(const Vec3 &N) const;
    double G(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;
    void sampleN(Vec3 &N, double &pdf, Sampler &RNG) const;

    Spectrum F0;
    double rough;
};

#ifdef ARC_IMPLEMENTATION

GGX::GGX(Spectrum F0, double rough) : F0(F0), rough(rough) {}
Spectrum GGX::F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const {
    return F0 + (Spectrum(1) - F0) * pow(1 - abs(L * N), 5);
}

double GGX::D(const Vec3 &N) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    return alpha2 / (pi * pow(N.z() * N.z() * (alpha2 - 1) + 1, 2));
}

double GGX::G(const Vec3 &V, const Vec3 &L, const Vec3 &N) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double tan_v = sqrt(1 - (V.z()) * (V.z())) / (V.z());
    double tan_l = sqrt(1 - (L.z()) * (L.z())) / (L.z());
    double G_v = 2 / (1 + sqrt(1 + alpha2 * tan_v * tan_v));
    double G_l = 2 / (1 + sqrt(1 + alpha2 * tan_l * tan_l));
    return max(G_v, 0.) * max(G_l, 0.);
}

void GGX::sampleN(Vec3 &N, double &pdf, Sampler &RNG) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double p = RNG.rand(), q = RNG.rand(-pi, pi);
    double cos_n = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_n = sqrt(1 - cos_n * cos_n);
    N = Vec3(cos(q) * sin_n, sin(q) * sin_n, cos_n);
    pdf = alpha2 * cos_n / (pi * pow((alpha2 - 1) * cos_n * cos_n + 1, 2));
}

Spectrum GGX::evaluate(const Vec3 &vLocal, const Vec3 &lLocal) const {
    if (vLocal.z() * lLocal.z() < 0) return Spectrum(0);
    Vec3 n = (vLocal + lLocal).norm();
    return F(vLocal, lLocal, n) * D(n) * G(vLocal, lLocal, n) / (4 * vLocal.z() * lLocal.z())
        + F0 * (1 - G(vLocal, lLocal, n)) / pi;
}

Spectrum GGX::sample(const Vec3 &vLocal, Vec3 &lLocal, double &pdf, Sampler &RNG) const {
    double prob = (3 - rough) / 3;
    if (RNG.rand() < prob) {
        Vec3 n;
        sampleN(n, pdf, RNG);
        lLocal = -vLocal + n * (vLocal * n) * 2;
        pdf = pdf / abs(4 * vLocal * n) * prob;
        if (vLocal.z() * lLocal.z() < 0) return Spectrum(0);
        return F(vLocal, lLocal, n) * D(n) * G(vLocal, lLocal, n) / (4 * vLocal.z() * lLocal.z());
    } else {
        lLocal = RNG.hemisphere();
        pdf = 1 / (2 * pi) * (1 - prob);
        Vec3 n = (vLocal + lLocal) / 2;
        return F0 * (1 - G(vLocal, lLocal, n)) / pi;
    }
}

double GGX::evaluatePdf(const Vec3 &vLocal, const Vec3 &lLocal) const {
    double prob = (3 - rough) / 3;
    if (vLocal.z() * lLocal.z() > 0) {
        Vec3 n = (vLocal + lLocal).norm();
        return D(n) * prob + 1 / (2 * pi) * (1 - prob);
    } else {
        return 0;
    }
}

#endif
#endif /* bxdfs_ggx_hpp */