#ifndef BxDF_hpp
#define BxDF_hpp

#include <utility>

#include "utils.hpp"
#include "sampler.hpp"
#include "photon.hpp"

enum BxDF_Type { REFLECTION, TRANSMISSION, DIFFUSE };
class BxDF
{

public:
    virtual void evaluate(const Vec3 &V, const Vec3 &L, double &weight)
    {
        throw invalid_argument("NotImplementedError");
    }
    virtual void sample(const Vec3 &V, Vec3 &L, double &pdf)
    {
        throw invalid_argument("NotImplementedError");
    }
};

class Lambert : public BxDF
{

public:
    Lambert() = default;
    void evaluate(const Vec3 &V, const Vec3 &L, double &weight) override;
    void sample(const Vec3 &V, Vec3 &L, double &pdf) override;
};

class GXX : public BxDF
{
    double ior, rough;

    double F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;
    double D(const Vec3 &N) const;
    double G(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;

public:
    GXX(double ior_, double rough_) : ior(ior_), rough(rough_) {}
    void evaluate(const Vec3 &V, const Vec3 &L, double &weight) override;
    void sample(const Vec3 &V, Vec3 &L, double &pdf) override;
};

#ifdef ARC_IMPLEMENTATION

void Lambert::evaluate(const Vec3 &V, const Vec3 &L, double &weight)
{
    if (L.d[2] * V.d[2] < 0) return (weight = 0, void());
    weight = 1. / pi * abs(L.d[2]);
}

void Lambert::sample(const Vec3 &V, Vec3 &L, double &pdf)
{
    L = RD.semisphere();
    if (V.d[2] < 0) L.d[2] *= -1;
    pdf = 1 / (2 * pi);
}

double GXX::F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const // @TODO Fresnel for GXX
{
    return 1;
}

double GXX::D(const Vec3 &N) const
{
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    return alpha2 / (pi * pow(N.d[2] * N.d[2] * (alpha2 - 1) + 1, 2));
}

double GXX::G(const Vec3 &V, const Vec3 &L, const Vec3 &N) const
{
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double tan_v = sqrt(1 - (V.d[2]) * (V.d[2])) / (V.d[2]);
    double tan_l = sqrt(1 - (L.d[2]) * (L.d[2])) / (L.d[2]);
    double G_v = 2 / (1 + sqrt(1 + alpha2 * tan_v * tan_v));
    double G_l = 2 / (1 + sqrt(1 + alpha2 * tan_l * tan_l));
    return max(G_v, 0.) * max(G_l, 0.);
}

void GXX::evaluate(const Vec3 &V, const Vec3 &L, double &weight)
{
    if (L.d[2] < 0) return (weight = 0, void());
    Vec3 N = (V + L).scale(1);
    weight = F(V, L, N) * D(N) * G(V, L, N) / (4 * V.d[2] * L.d[2]) * L.d[2];
}

void GXX::sample(const Vec3 &V, Vec3 &L, double &pdf)
{
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double p = RD.rand(), q = RD.rand(-pi, pi);
    double cos_n = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_n = sqrt(1 - cos_n * cos_n);
    Vec3 normal(cos(q) * sin_n, sin(q) * sin_n, cos_n);

    L = -V + normal * (V * normal) * 2;
    pdf = alpha2 * cos_n / (pi * pow((alpha2 - 1) * cos_n * cos_n + 1, 2));
}

#endif
#endif /* BxDF_hpp */
