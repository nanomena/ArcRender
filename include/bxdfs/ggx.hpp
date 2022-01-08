#ifndef bxdfs_ggx_hpp
#define bxdfs_ggx_hpp

#include "../bxdf.hpp"

class GGX : public BxDF {
public:
    GGX(Spectrum F0, double roughness);

private:
    Spectrum evaluate(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const override;
    Spectrum sample(const Vec2 &texPos, const Vec3 &i, Vec3 &o, double &pdf, Sampler &RNG) const override;
    double evaluateImportance(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const override;

    Spectrum F(const Vec3 &i, const Vec3 &o, const Vec3 &n) const;
    double D(const Vec3 &n) const;
    double G(const Vec3 &i, const Vec3 &o, const Vec3 &n) const;
    void sampleN(Vec3 &n, double &pdf, Sampler &RNG) const;

    Spectrum F0;
    double rough;
};

#ifdef ARC_IMPLEMENTATION

GGX::GGX(Spectrum F0, double roughness) : F0(F0), rough(roughness) {}
Spectrum GGX::F(const Vec3 &i, const Vec3 &o, const Vec3 &n) const {
    return F0 + (Spectrum(1) - F0) * pow(1 - abs(o * n), 5);
}

double GGX::D(const Vec3 &n) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    return alpha2 / (pi * pow(n.z() * n.z() * (alpha2 - 1) + 1, 2));
}

double GGX::G(const Vec3 &i, const Vec3 &o, const Vec3 &n) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double tan_v = sqrt(1 - (i.z()) * (i.z())) / (i.z());
    double tan_l = sqrt(1 - (o.z()) * (o.z())) / (o.z());
    double G_v = 2 / (1 + sqrt(1 + alpha2 * tan_v * tan_v));
    double G_l = 2 / (1 + sqrt(1 + alpha2 * tan_l * tan_l));
    return max(G_v, 0.) * max(G_l, 0.);
}

void GGX::sampleN(Vec3 &n, double &pdf, Sampler &RNG) const {
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double p = RNG.rand(), q = RNG.rand(-pi, pi);
    double cos_n = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_n = sqrt(1 - cos_n * cos_n);
    n = Vec3(cos(q) * sin_n, sin(q) * sin_n, cos_n);
    pdf = alpha2 * cos_n / (pi * pow((alpha2 - 1) * cos_n * cos_n + 1, 2));
}

Spectrum GGX::evaluate(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const {
    if (i.z() * o.z() < 0) return Spectrum(0);
    Vec3 n = (i + o).norm();
    return F(i, o, n) * D(n) * G(i, o, n) / (4 * i.z() * o.z())
        + F0 * (1 - G(i, o, n)) / pi;
}

Spectrum GGX::sample(const Vec2 &texPos, const Vec3 &i, Vec3 &o, double &pdf, Sampler &RNG) const {
    double prob = (3 - rough) / 3;
    if (RNG.rand() < prob) {
        Vec3 n;
        sampleN(n, pdf, RNG);
        o = -i + n * (i * n) * 2;
        pdf = pdf / abs(4 * i * n) * prob;
        if (i.z() * o.z() < 0) return Spectrum(0);
        return F(i, o, n) * D(n) * G(i, o, n) / (4 * i.z() * o.z());
    } else {
        o = RNG.hemisphere();
        pdf = 1 / (2 * pi) * (1 - prob);
        Vec3 n = (i + o) / 2;
        return F0 * (1 - G(i, o, n)) / pi;
    }
}

double GGX::evaluateImportance(const Vec2 &texPos, const Vec3 &i, const Vec3 &o) const {
    double prob = (3 - rough) / 3;
    if (i.z() * o.z() > 0) {
        Vec3 n = (i + o).norm();
        return D(n) * prob + 1 / (2 * pi) * (1 - prob);
    } else {
        return 0;
    }
}

#endif
#endif /* bxdfs_ggx_hpp */