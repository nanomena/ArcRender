#ifndef BxDF_hpp
#define BxDF_hpp

#include "utils.hpp"
#include "sampler.hpp"
#include "surface.hpp"
#include "photon.hpp"

class BxDF
{

public:
    virtual double forward(const sInfo &S, Photon &photon, int &type) const;
};

class BSDF : public BxDF
{
    static double F(const sInfo &S, double cos_i, double cos_t);
    static double Fd(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal);
    static Vec3 sampleD(const sInfo &S);
    static double D(const sInfo &S, const Vec3 &normal);
    static double Gl(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal);
    static double Gt(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal);
    static double through(const sInfo &S, const Vec3 &in, Vec3 &out, Spectrum &spectrum, int &type);

public:
    double forward(const sInfo &S, Photon &photon, int &type) const override;
};

#ifdef ARC_IMPLEMENTATION

double BxDF::forward(const sInfo &S, Photon &photon, int &type) const
{
    throw invalid_argument("NotImplementedError");
}

double BSDF::F(const sInfo &S, double cos_i, double cos_t)
{
    double rs = (S.ior * cos_i - cos_t) / (S.ior * cos_i + cos_t);
    double rp = (S.ior * cos_t - cos_i) / (S.ior * cos_t + cos_i);
    return (rs * rs + rp * rp) * 0.5;
}
double BSDF::Fd(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal)
{
    double cos_i = -in * normal;
    double f_in = pow(1 - -in.d[2], 5);
    double f_out = pow(1 - out.d[2], 5);
    double r_r = 2 * cos_i * cos_i * S.rough;

    double f_l = (1 - 0.5 * f_in) * (1 - 0.5 * f_out);
    double f_r = r_r * (f_in + f_out + f_in * f_out * (r_r - 1));

    double f = f_l + f_r; // It's wrong ~, but it does work well.
    return f;
}

double BSDF::D(const sInfo &S, const Vec3 &normal)
{
    double no_no = normal.d[2] * normal.d[2];
    double alpha2 = pow(S.rough, 4);
    return alpha2 / (pi * (no_no * (alpha2 - 1) + 1) * (no_no * (alpha2 - 1) + 1));
}

double BSDF::Gl(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal)
{
    double alpha = S.rough * S.rough;
    double k = alpha / 4;
    double g_in = -in.d[2] / (-in.d[2] * (1 - k) + k);
    double g_out = out.d[2] / (out.d[2] * (1 - k) + k);
    double g = g_in * g_out;
    return g;
}
double BSDF::Gt(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal)
{
    double alpha = S.rough * S.rough;
    double k = alpha / 4;
    double g_in = -in.d[2] / (-in.d[2] * (1 - k) + k);
    double g_out = -out.d[2] / (-out.d[2] * (1 - k) + k);
    double g = g_in * g_out;

    double cos_i = -in * normal;
    double cos_t = -out * normal;
    return g * cos_i * cos_t / pow((cos_i + cos_t / S.ior) / 2, 2);
}
Vec3 BSDF::sampleD(const sInfo &S)
{
    double alpha2 = pow(S.rough, 4);
    double p = RD.rand();
    double cos_theta = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_theta = sqrt(1 - cos_theta * cos_theta);
    double q = RD.rand(-pi, pi);
    return Vec3(cos(q) * sin_theta, sin(q) * sin_theta, cos_theta);
}

double BSDF::through(const sInfo &S, const Vec3 &in, Vec3 &out, Spectrum &spectrum, int &type)
{
    if (RD.rand() < S.absorb)
    {
        spectrum = S.emission;
        type = 0;
        return 1;
    }

    Vec3 normal = sampleD(S);
    double cos_i = -in * normal, cos_t;

    if (cos_i < 0)
    {
        spectrum = Spectrum(0);
        return 0;
    }
    double reflect;
    if (sqrt(1 - cos_i * cos_i) * S.ior > 1) reflect = 1;
    else
    {
        cos_t = sqrt(1 - (1 - cos_i * cos_i) * S.ior * S.ior);
        reflect = F(S, cos_i, cos_t);
    }
    if (RD.rand() < reflect)
    {
        out = in + normal * cos_i * 2;
        if (out.d[2] < 0)
        {
            spectrum = Spectrum(0);
            return 0;
        }
        else
        {
            type = 2;
            spectrum = S.specular * Gl(S, in, out, normal);
            return 1;
        }
    }
    else
    {
        if (RD.rand() < S.diffuse)
        {
            type = 1;
            out = RD.semisphere();
            normal = (out - in).scale(1);
            spectrum = S.base * Fd(S, in, out, normal);
            return 1;
        }
        else
        {
            out = (in * S.ior + normal * (cos_i * S.ior - cos_t)).scale(1);
            if (out.d[2] > 0 || RD.rand() > Gt(S, in, out, normal))
            {
                type = -1;
                out = RD.semisphere();
                out.d[2] *= -1;
                spectrum = S.base;
                return 1;
            }
            else
            {
                type = -2;
                spectrum = Spectrum(1);
                return 1;
            }
        }
    }
}

double BSDF::forward(const sInfo &S, Photon &photon, int &type) const
{
    Vec3 in = photon.ray.d, out;
    Spectrum spectrum;
    double weight = through(S, in, out, spectrum, type);
    photon.ray.o = Vec3();
    photon.ray.d = out;
    photon.trans(spectrum);
    if (type < 0) photon.into(S.inside);
    return weight;
}

#endif
#endif /* BxDF_hpp */
