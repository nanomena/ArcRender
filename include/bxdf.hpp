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

class GGX : public BxDF
{
    double ior, rough;

    double F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;
    double D(const Vec3 &N) const;
    double G(const Vec3 &V, const Vec3 &L, const Vec3 &N) const;

public:
    GGX(double ior_, double rough_) : ior(ior_), rough(rough_) {}
    void evaluate(const Vec3 &V, const Vec3 &L, double &weight) override;
    void sample(const Vec3 &V, Vec3 &L, double &pdf) override;
};

#ifdef ARC_IMPLEMENTATION

void Lambert::evaluate(const Vec3 &V, const Vec3 &L, double &weight)
{
    if (L.d[2] * V.d[2] < 0) return (weight = 0, void());
    weight = 1. / pi;
}

void Lambert::sample(const Vec3 &V, Vec3 &L, double &pdf)
{
    L = RD.semisphere();
    if (V.d[2] < 0) L.d[2] *= -1;
    pdf = 1 / (2 * pi);
}

double GGX::F(const Vec3 &V, const Vec3 &L, const Vec3 &N) const
{
    double F0 = pow((ior - 1) / (ior + 1), 2);
    return F0 + (1 - F0) * pow(1 - (V * N), 5);
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

void GGX::evaluate(const Vec3 &V, const Vec3 &L, double &weight)
{
    if (L.d[2] < 0 || V.d[2] < 0) return (weight = 0, void());
    Vec3 N = (V + L).scale(1);
    weight = (F(V, L, N) * D(N) * G(V, L, N) / (4 * V.d[2] * L.d[2]) + (1 - F(V, L, N)) / pi);
}

void GGX::sample(const Vec3 &V, Vec3 &L, double &pdf)
{
    double F0 = pow((ior - 1) / (ior + 1), 2);
    double alpha = rough * rough;
    double alpha2 = alpha * alpha;
    double p = RD.rand(), q = RD.rand(-pi, pi);
    double cos_n = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_n = sqrt(1 - cos_n * cos_n);
    Vec3 normal(cos(q) * sin_n, sin(q) * sin_n, cos_n);

    if (RD.rand() < F0)
        L = -V + normal * (V * normal) * 2;
    else
        L = RD.semisphere();
    pdf = alpha2 * cos_n / (pi * pow((alpha2 - 1) * cos_n * cos_n + 1, 2)) * F0 + (1 / (2 * pi)) * (1 - F0);
}

#endif
#endif /* BxDF_hpp */
