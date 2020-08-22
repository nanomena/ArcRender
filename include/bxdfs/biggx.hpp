#ifndef bxdfs_biggx_hpp
#define bxdfs_biggx_hpp

#include "../bxdf.hpp"

class BiGGX : public BxDF
{
    double ior, rough, diffuse;

    double F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;
    double D(const Vec3 &N) const;
    double G(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;
    void sample_N(Vec3 &N, double &pdf) const;

public:
    BiGGX(double ior_, double rough_, double diffuse_) : ior(ior_), rough(rough_), diffuse(diffuse_) {}
    void evaluate(const Vec3 &V, const Vec3 &L, double &weight) override;
    void sample_VtL(const Vec3 &V, Vec3 &L, double &pdf) override;
    void sample_LtV(const Vec3 &L, Vec3 &V, double &pdf) override;
};

#ifdef ARC_IMPLEMENTATION

double BiGGX::F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const
{
    double F0 = (L.d[2] > 0 ? pow((ior - 1) / (ior + 1), 2) : pow((1 / ior - 1) / (1 / ior + 1), 2));
    return F0 + (1 - F0) * pow(1 - (L * N), 5);
}

double BiGGX::D(const Vec3 &N) const
{
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    return alpha2 / (pi * pow(N.d[2] * N.d[2] * (alpha2 - 1) + 1, 2));
}

double BiGGX::G(const Vec3 &V, const Vec3 &L, const Vec3 &N) const
{
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double tan_v = (V.d[2] > 0 ? 1 : -1) * sqrt(1 - (V.d[2]) * (V.d[2])) / (V.d[2]);
    double tan_l = (L.d[2] > 0 ? 1 : -1) * sqrt(1 - (L.d[2]) * (L.d[2])) / (L.d[2]);
    double G_v = 2 / (1 + sqrt(1 + alpha2 * tan_v * tan_v));
    double G_l = 2 / (1 + sqrt(1 + alpha2 * tan_l * tan_l));
    return max(G_v, 0.) * max(G_l, 0.);
}

void BiGGX::sample_N(Vec3 &N, double &pdf) const
{
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double p = RD.rand(), q = RD.rand(-pi, pi);
    double cos_n = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_n = sqrt(1 - cos_n * cos_n);
    N = Vec3(cos(q) * sin_n, sin(q) * sin_n, cos_n);
    pdf = alpha2 * cos_n / (pi * pow((alpha2 - 1) * cos_n * cos_n + 1, 2));
}

void BiGGX::evaluate(const Vec3 &V, const Vec3 &L, double &weight)
{
    if (V.d[2] * L.d[2] > 0)
    {
        Vec3 N = (V + L).scale(1);
        weight = (F(V, L, N) * D(N) * G(V, L, N) / (4 * V.d[2] * L.d[2]) + (1 - F(V, L, N)) * diffuse / pi);
    }
    else
    {
        Vec3 N = (-V - L * (L.d[2] > 0 ? 1 / ior : ior)).scale(1);
        if (L * N < 0) N = -N;
        weight = (1 - F(V, L, N)) * (1 - diffuse) * D(N) * G(V, L, N) * (abs(L * N) * abs(V * N))
            / pow((L.d[2] > 0 ? 1 / ior : ior) * (L * N) + V * N, 2) / abs(V.d[2] * L.d[2]);
    }
}

void BiGGX::sample_VtL(const Vec3 &V, Vec3 &L, double &pdf)
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
    else if (RD.rand() < diffuse)
    {
        L = RD.semisphere();
        pdf = (1 / (2 * pi));
    }
    else
    {
        Vec3 N;
        sample_N(N, pdf);
        if (V.d[2] > 0) N = -N;
        double cos_v = -V * N, sin_v = sqrt(1 - cos_v * cos_v),
            sin_l = sin_v * (V.d[2] > 0 ? 1 / ior : ior), cos_l = sqrt(1 - sin_l * sin_l); // @FIXME  ? ? ?
        L = (-V * (V.d[2] > 0 ? 1 / ior : ior) + N * (cos_l - cos_v * (V.d[2] > 0 ? 1 / ior : ior))).scale(1);
        pdf = pdf * (abs(L * N) * abs(V * N))
            / pow((L.d[2] > 0 ? 1 / ior : ior) * (L * N) + V * N, 2) / abs(V.d[2] * L.d[2]);
    }
}

void BiGGX::sample_LtV(const Vec3 &L, Vec3 &V, double &pdf)
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
    else if (RD.rand() < diffuse)
    {
        V = RD.semisphere();
        pdf = (1 / (2 * pi));
    }
    else
    {
        Vec3 N;
        sample_N(N, pdf);
        if (L.d[2] < 0) N = -N;
        double cos_l = L * N, sin_l = sqrt(1 - cos_l * cos_l),
            sin_v = sin_l * (L.d[2] < 0 ? ior : 1 / ior), cos_v = sqrt(1 - sin_l * sin_l);
        V = (-L * (L.d[2] < 0 ? ior : 1 / ior) + N * (cos_v - cos_l * (L.d[2] < 0 ? ior : 1 / ior))).scale(1);
        pdf = pdf * (abs(L * N) * abs(V * N))
            / pow((L.d[2] > 0 ? 1 / ior : ior) * (L * N) + V * N, 2) / abs(V.d[2] * L.d[2]);
    }
}


#endif
#endif /* bxdfs_biggx_hpp */
