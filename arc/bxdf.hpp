#ifndef BXDF
#define BXDF
#include "sampler.hpp"

class BxDF
{

public:
    BxDF () {}

    double virtual through(const Vec3 &in, Vec3 &out, Spectrum &spectrum) const
        { throw "NotImplementedError"; }
};

class BRDF : public BxDF
{
    double rough, metal;
    Spectrum diffuse, specular, albedo;
    double alpha, alpha2, prob;

public:
    BRDF (Spectrum _albedo, double _rough, double _metal)
    {
        albedo = _albedo;
        rough = _rough;
        metal = _metal;

        alpha = rough * rough;
        alpha2 = alpha * alpha;

        diffuse = albedo * (1 - metal);
        specular = Spectrum(0.04) * (1 - metal) + albedo * metal;

        prob = specular.norm() / (diffuse.norm() + specular.norm());
    }

    Spectrum F(const Vec3 &in, const Vec3 &normal) const
    {
        return specular + (Spectrum(1) - specular) * pow(1 - (-in) * normal, 5);
    }

    double D(const Vec3 &normal) const
    {
        double nz2 = normal.d[2] * normal.d[2];
        return alpha2 / (pi * (nz2 * (alpha2 - 1) + 1) * (nz2 * (alpha2 - 1) + 1));
    }

    double G(const Vec3 &in, const Vec3 &out) const
    {
        // return 1;

        // return 1 / max(-in.d[2], out.d[2]);

        double k = (rough + 1) * (rough + 1) / 8;
        double g_in = -in.d[2] / (-in.d[2] * (1 - k) + k);
        double g_out = out.d[2] / (out.d[2] * (1 - k) + k);
        return g_in * g_out / (-in.d[2] * out.d[2]);

        // double _g_in = -in.d[2] * sqrt(alpha2 + out.d[2] * (out.d[2] - alpha2 * out.d[2]));
        // double _g_out = out.d[2] * sqrt(alpha2 + -in.d[2] * (-in.d[2] - alpha2 * -in.d[2]));
        // return 2 / (_g_in + _g_out);
    }

    Spectrum FD(const Vec3 &in, const Vec3 &out, const Vec3 &normal) const
    {
        double cosd = (-in) * normal;
        double fd90 = 0.5 + 2 * cosd * cosd * rough;
        double f_in = 1 + (fd90 - 1) * pow(1 - -in.d[2], 5);
        double f_out = 1 + (fd90 - 1) * pow(1 - out.d[2], 5);
        // cerr << f_in * f_out << endl;
        return diffuse * f_in * f_out;
    }

    double through(const Vec3 &in, Vec3 &out, Spectrum &spectrum) const
    {
        if (RD.rand() < prob)
        {
            double p = RD.rand();
            double costheta = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
            double sintheta = sqrt(1 - costheta * costheta);
            double q = RD.rand(-pi, pi);

            Vec3 normal = Vec3(cos(q) * sintheta, sin(q) * sintheta, costheta);

            out = in - normal * (normal * in) * 2;
            if (out.d[2] < 0)
            {
                spectrum = Spectrum(0);
                return 0;
            }

            spectrum = F(in, normal) / prob;

            return G(in, out) / 4;
        }
        else
        {
            out = RD.semisphere();
            Vec3 normal = (out - in).scale(1);
            spectrum = FD(in, normal, out) / (1 - prob);

            return 1 / pi;
        }
    }
};

#endif
