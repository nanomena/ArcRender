#ifndef bxdfs_ggx_hpp
#define bxdfs_ggx_hpp

#include "../bxdf.hpp"

class GGX : public BxDF
{
    double ior, rough;

    double F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;
    double D(const Vec3 &N) const;
    double G(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;
    void sample_N(Vec3 &N, double &pdf) const;

public:
    GGX(double ior_, double rough_) : ior(ior_), rough(rough_) {}
    void evaluate(const Vec3 &V, const Vec3 &L, double &weight) override;
    void sample_VtL(const Vec3 &V, Vec3 &L, double &pdf) override;
    void sample_LtV(const Vec3 &L, Vec3 &V, double &pdf) override;
};

#ifdef ARC_IMPLEMENTATION

double GGX::F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const
{
    double F0 = pow((ior - 1) / (ior + 1), 2);
    return F0 + (1 - F0) * pow(1 - (L * N), 5);
}

double GGX::D(const Vec3 &N) const
{
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    return alpha2 / (pi * pow(N.d[2] * N.d[2] * (alpha2 - 1) + 1, 2));
}

double GGX::G(const Vec3 &V, const Vec3 &L, const Vec3 &N) const
{
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double tan_v = sqrt(1 - (V.d[2]) * (V.d[2])) / (V.d[2]);
    double tan_l = sqrt(1 - (L.d[2]) * (L.d[2])) / (L.d[2]);
    double G_v = 2 / (1 + sqrt(1 + alpha2 * tan_v * tan_v));
    double G_l = 2 / (1 + sqrt(1 + alpha2 * tan_l * tan_l));
    return max(G_v, 0.) * max(G_l, 0.);
}

void GGX::sample_N(Vec3 &N, double &pdf) const
{
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double p = RD.rand(), q = RD.rand(-pi, pi);
    double cos_n = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_n = sqrt(1 - cos_n * cos_n);
    N = Vec3(cos(q) * sin_n, sin(q) * sin_n, cos_n);
    pdf = alpha2 * cos_n / (pi * pow((alpha2 - 1) * cos_n * cos_n + 1, 2));
}

void GGX::evaluate(const Vec3 &V, const Vec3 &L, double &weight)
{
    if (L.d[2] < 0 || V.d[2] < 0) return (weight = 0, void());
    Vec3 N = (V + L).scale(1);
    weight = (F(V, L, N) * D(N) * G(V, L, N) / (4 * V.d[2] * L.d[2]) + (1 - F(V, L, N)) / pi);
}

void GGX::sample_VtL(const Vec3 &V, Vec3 &L, double &pdf)
{
    double F0 = pow((ior - 1) / (ior + 1), 2);
    if (RD.rand() < F0)
    {
        Vec3 N;
        sample_N(N, pdf);
        L = -V + N * (V * N) * 2;
        pdf = pdf / (V * N * 4);
        if (pdf < 0) pdf = 0;
    }
    else
    {
        L = RD.semisphere();
        pdf = (1 / (2 * pi));
    }
}

void GGX::sample_LtV(const Vec3 &L, Vec3 &V, double &pdf)
{
    double F0 = pow((ior - 1) / (ior + 1), 2);
    if (RD.rand() < F0)
    {
        Vec3 N;
        sample_N(N, pdf);
        V = -L + N * (L * N) * 2;
        pdf = pdf / (L * N * 4);
        if (pdf < 0) pdf = 0;
    }
    else
    {
        V = RD.semisphere();
        pdf = (1 / (2 * pi));
    }
}


#endif
#endif /* bxdfs_ggx_hpp */