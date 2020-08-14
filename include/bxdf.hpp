#ifndef BxDF_hpp
#define BxDF_hpp

#include "utils.hpp"
#include "sampler.hpp"
#include "surface.hpp"
#include "photon.hpp"

enum hit_type { mirror_refr, diff_refr, source, diff_refl, mirror_refl };

class BxDF
{

public:
    virtual double forward(const sInfo &S, Photon &photon, hit_type &type) const;
};

class BSDF : public BxDF
{
    static double F(const sInfo &S, double cos_i, double cos_t);
    static double D(const sInfo &S, const Vec3 &normal);
    static double G(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal);
    static Vec3 sampleD(const sInfo &S);

    static double Fd(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal);
    static double through(const sInfo &S, const Vec3 &in, Vec3 &out, Spectrum &spectrum, hit_type &type);

public:
    double forward(const sInfo &S, Photon &photon, hit_type &type) const override;
};

class BSDF_Thin : public BxDF
{
    static double F(const sInfo &S, double cos_i, double cos_t);
    static double D(const sInfo &S, const Vec3 &normal);
    static double G(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal);
    static Vec3 sampleD(const sInfo &S);

    static double Fd(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal);
    static double through(const sInfo &S, const Vec3 &in, Vec3 &out, Spectrum &spectrum, hit_type &type);

public:
    double forward(const sInfo &S, Photon &photon, hit_type &type) const override;
};

#ifdef ARC_IMPLEMENTATION

double BxDF::forward(const sInfo &S, Photon &photon, hit_type &type) const
{
    throw invalid_argument("NotImplementedError");
}

double BSDF::F(const sInfo &S, double cos_i, double cos_t)
{
    double rs = (S.ior * cos_i - cos_t) / (S.ior * cos_i + cos_t);
    double rp = (S.ior * cos_t - cos_i) / (S.ior * cos_t + cos_i);
    return (rs * rs + rp * rp) * 0.5;
}

double BSDF::D(const sInfo &S, const Vec3 &normal)
{
    double alpha = S.rough * S.rough;
    double alpha2 = alpha * alpha;
    return alpha2 / (pi * pow(normal.d[2] * normal.d[2] * (alpha2 - 1) + 1, 2));
}

double BSDF::G(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal)
{
    double alpha = S.rough * S.rough;
    double alpha2 = alpha * alpha;
    double t_i = sqrt(1 - (-in.d[2]) * (-in.d[2])) / (-in.d[2]);
    double t_t = sqrt(1 - (out.d[2]) * (out.d[2])) / (out.d[2]);
    double g_i = 2 / (1 + sqrt(1 + alpha2 * t_i * t_i));
    double g_t = 2 / (1 + sqrt(1 + alpha2 * t_t * t_t));
    return g_i * g_t;
}

double BSDF::Fd(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal)
{
    double cos_i = -in * normal;
    double Fd90 = 0.5 + (2 * S.rough * cos_i * cos_i);
    double f_i = 1 + (Fd90 - 1) * pow(1 - -in.d[2], 5);
    double f_t = 1 + (Fd90 - 1) * pow(1 - out.d[2], 5);
    assert(f_i > 0);
    assert(f_t > 0);
    return f_i * f_t;
}

Vec3 BSDF::sampleD(const sInfo &S)
{
    double alpha = S.rough * S.rough;
    double alpha2 = alpha * alpha;
    double p = RD.rand();
    double cos_theta = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_theta = sqrt(1 - cos_theta * cos_theta);
    double q = RD.rand(-pi, pi);
    return Vec3(cos(q) * sin_theta, sin(q) * sin_theta, cos_theta);
}

double BSDF::through(const sInfo &S, const Vec3 &in, Vec3 &out, Spectrum &spectrum, hit_type &type)
{
    if (RD.rand() < S.absorb)
    {
        spectrum = S.emission;
        type = source;
        return 1;
    }

    Vec3 normal = sampleD(S);
    double cos_i = -in * normal, cos_t;
    if (cos_i < 0)
    {
        spectrum = Spectrum(0);
        type = source;
        return 0;
    }
    double reflect;
    if (sqrt(1 - cos_i * cos_i) * S.ior > 1) reflect = 1;
    else cos_t = sqrt(1 - (1 - cos_i * cos_i) * S.ior * S.ior), reflect = F(S, cos_i, cos_t);

    if (RD.rand() < reflect)
    {
        out = in + normal * cos_i * 2;
        if (out.d[2] < 0)
        {
            spectrum = Spectrum(0);
            type = source;
            return 1;
        }
        type = mirror_refl;
        spectrum = S.specular * G(S, in, out, normal);
        return 1;
    }
    else if (RD.rand() < S.diffuse)
    {
        out = RD.semisphere();
        type = diff_refl;
        spectrum = S.base * Fd(S, in, out, normal);
        return 1;
    }
    else
    {
        out = (in * S.ior + normal * (cos_i * S.ior - cos_t)).scale(1);
        if (out.d[2] > 0)
        {
            spectrum = Spectrum(0);
            type = source;
            return 1;
        }
        type = mirror_refr;
        spectrum = Spectrum(1) * G(S, in, out, normal);
        return 1;
    }
}

double BSDF::forward(const sInfo &S, Photon &photon, hit_type &type) const
{
    assert(S.rough >= 0);
    Vec3 in = photon.ray.d, out;
    Spectrum spectrum;
    double weight = through(S, in, out, spectrum, type);
    photon.ray.o = Vec3();
    photon.ray.d = out;
    photon.trans(spectrum);
    if ((type == mirror_refr) || (type == diff_refr)) photon.into(S.inside);
    return weight;
}

double BSDF_Thin::F(const sInfo &S, double cos_i, double cos_t)
{
    double rs = (S.ior * cos_i - cos_t) / (S.ior * cos_i + cos_t);
    double rp = (S.ior * cos_t - cos_i) / (S.ior * cos_t + cos_i);
    return (rs * rs + rp * rp) * 0.5;
}

double BSDF_Thin::D(const sInfo &S, const Vec3 &normal)
{
    double alpha = S.rough * S.rough;
    double alpha2 = alpha * alpha;
    return alpha2 / (pi * pow(normal.d[2] * normal.d[2] * (alpha2 - 1) + 1, 2));
}

double BSDF_Thin::G(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal)
{
    double alpha = S.rough * S.rough;
    double alpha2 = alpha * alpha;
    double t_i = sqrt(1 - (-in.d[2]) * (-in.d[2])) / (-in.d[2]);
    double t_t = sqrt(1 - (out.d[2]) * (out.d[2])) / (out.d[2]);
    double g_i = 2 / (1 + sqrt(1 + alpha2 * t_i * t_i));
    double g_t = 2 / (1 + sqrt(1 + alpha2 * t_t * t_t));
    return g_i * g_t;
}

double BSDF_Thin::Fd(const sInfo &S, const Vec3 &in, const Vec3 &out, const Vec3 &normal)
{
    double cos_i = -in * normal;
    double Fd90 = 0.5 + (2 * S.rough * cos_i * cos_i);
    double f_i = 1 + (Fd90 - 1) * pow(1 - -in.d[2], 5);
    double f_t = 1 + (Fd90 - 1) * pow(1 - out.d[2], 5);
    assert(f_i > 0);
    assert(f_t > 0);
    return f_i * f_t;
}

Vec3 BSDF_Thin::sampleD(const sInfo &S)
{
    double alpha = S.rough * S.rough;
    double alpha2 = alpha * alpha;
    double p = RD.rand();
    double cos_theta = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
    double sin_theta = sqrt(1 - cos_theta * cos_theta);
    double q = RD.rand(-pi, pi);
    return Vec3(cos(q) * sin_theta, sin(q) * sin_theta, cos_theta);
}

double BSDF_Thin::through(const sInfo &S, const Vec3 &in, Vec3 &out, Spectrum &spectrum, hit_type &type)
{
    if (RD.rand() < S.absorb)
    {
        spectrum = S.emission;
        type = source;
        return 1;
    }

    Vec3 normal = sampleD(S);
    double cos_i = -in * normal, cos_t;
    if (cos_i < 0)
    {
        spectrum = Spectrum(0);
        type = source;
        return 0;
    }
    double reflect;
    if (sqrt(1 - cos_i * cos_i) * S.ior > 1) reflect = 1;
    else cos_t = sqrt(1 - (1 - cos_i * cos_i) * S.ior * S.ior), reflect = F(S, cos_i, cos_t);

    if (RD.rand() < reflect)
    {
        out = in + normal * cos_i * 2;
        if (out.d[2] < 0 || RD.rand() < G(S, in, out, normal))
        {
            out = RD.semisphere();
            type = diff_refl;
            spectrum = S.base * Fd(S, in, out, normal);
            return 1;
        }
        else
        {
            type = mirror_refl;
            spectrum = S.specular;
            return 1;
        }
    }
    else if (RD.rand() < S.diffuse)
    {
        out = RD.semisphere();
        type = diff_refl;
        spectrum = S.base * Fd(S, in, out, normal);
        return 1;
    }
    else
    {
        if (RD.rand() < G(S, in, out, normal))
        {
            out = RD.semisphere();
            out.d[2] *= -1;
            type = diff_refr;
            spectrum = S.base * Fd(S, in, out, normal);
            return 1;
        }
        else
        {
            out = in;
            type = mirror_refr;
            spectrum = Spectrum(1);
            return 1;
        }
    }
}

double BSDF_Thin::forward(const sInfo &S, Photon &photon, hit_type &type) const
{
    assert(S.rough >= 0);
    Vec3 in = photon.ray.d, out;
    Spectrum spectrum;
    double weight = through(S, in, out, spectrum, type);
    photon.ray.o = Vec3();
    photon.ray.d = out;
    photon.trans(spectrum);
    return weight;
}

#endif
#endif /* BxDF_hpp */
