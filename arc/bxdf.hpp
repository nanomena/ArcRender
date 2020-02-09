#ifndef BXDF
#define BXDF
#include "arc.hpp"
#include "sampler.hpp"
#include "material.hpp"
#include "photon.hpp"

class BxDF
{

public:
    BxDF () {}

    double virtual through(const Vec3 &in, Vec3 &out, Spectrum &spectrum, int &type) const
        { throw "NotImplementedError"; }
};

class Metal : public BxDF
{
    double rough, metal;
    Spectrum diffuse, specular, albedo;
    double alpha, alpha2, prob;

public:
    Metal (Spectrum _albedo, double _rough, double _metal)
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
        double k = alpha / 4;
        double g_in = -in.d[2] / (-in.d[2] * (1 - k) + k);
        double g_out = out.d[2] / (out.d[2] * (1 - k) + k);
        double g = g_in * g_out;
        return g;

        // double c_i = (-in * normal), t_i2 = (1 - c_i * c_i) / (c_i * c_i);
        // double g_i = max(0., 2 * (-in * normal) / (-in.d[2]) / (1 + sqrt(1 + alpha2 * t_i2)));
        // double c_o = (out * normal), t_o2 = (1 - c_o * c_o) / (c_o * c_o);
        // double g_o = max(0., 2 * (out * normal) / (out.d[2]) / (1 + sqrt(1 + alpha2 * t_o2)));
        // double g = g_i * g_o * (-in.d[2]) * (out.d[2]);
        // return g;
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

    double through(const Vec3 &in, Vec3 &out, Spectrum &spectrum, int &type) const
    {
        if (RD.rand() < prob)
        {
            type = 0;

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

            spectrum = F(in, normal) * G(in, out) / prob;

            return 1;
        }
        else
        {
            type = 1;

            out = RD.semisphere();
            Vec3 normal = (out - in).scale(1);
            spectrum = FD(in, normal, out) / (1 - prob);

            return 1;
        }
    }
};


class Trans : public BxDF
{
    double IOR, rough;
    Spectrum specular;
    double alpha, alpha2;

public:
    Trans (double _IOR, double _rough)
    {
        IOR = _IOR;
        rough = _rough;

        alpha = rough * rough;
        alpha2 = alpha * alpha;

        specular = Spectrum(pow((IOR - 1) / (IOR + 1), 2) * 2);
    }

    double F(double cosi, double cost) const
    {
        double rs = (IOR * cosi - cost) / (IOR * cosi + cost);
        double rp = (IOR * cost - cosi) / (IOR * cost + cosi);
        return (rs * rs + rp * rp) * 0.5;
    }

    double D(const Vec3 &normal) const
    {
        double nz2 = normal.d[2] * normal.d[2];
        return alpha2 / (pi * (nz2 * (alpha2 - 1) + 1) * (nz2 * (alpha2 - 1) + 1));
    }

    double G(const Vec3 &in, const Vec3 &out) const
    {
        double k = alpha / 4;
        double g_in = -in.d[2] / (-in.d[2] * (1 - k) + k);
        double g_out = out.d[2] / (out.d[2] * (1 - k) + k);
        double g = g_in * g_out;
        return g;
    }

    double GT(const Vec3 &in, const Vec3 &out, const Vec3 &normal) const
    {
        double k = alpha / 4;
        double g_in = -in.d[2] / (-in.d[2] * (1 - k) + k);
        double g_out = -out.d[2] / (-out.d[2] * (1 - k) + k);
        double g = g_in * g_out;

        double i_in = -in * normal;
        double i_out = -out * normal;
        return g * i_in * i_out / pow((i_in + i_out / IOR) / 2, 2);
    }

    double through(const Vec3 &in, Vec3 &out, Spectrum &spectrum, int &type) const
    {
        double p = RD.rand();
        double costheta = sqrt((1 - p) / (p * (alpha2 - 1) + 1));
        double sintheta = sqrt(1 - costheta * costheta);
        double q = RD.rand(-pi, pi);

        Vec3 normal = Vec3(cos(q) * sintheta, sin(q) * sintheta, costheta);

        double cosi = -in * normal, cost;

        if (cosi < 0)
        {
            spectrum = Spectrum(0);
            return 0;
        }
        double reflect;
        if (sqrt(1 - cosi * cosi) * IOR > 1) reflect = 1;
        else
        {
            cost = sqrt(1 - (1 - cosi * cosi) * IOR * IOR);
            reflect = F(cosi, cost);
            // cerr << reflect << endl;

            // reflect = 0; //
        }
        // cerr << "NMDWSM " << cosi << " " << cost << " " << reflect << endl;


        if (RD.rand() < reflect)
        {
            type = 0;

            $ << "reflect !" << endl;
            out = in + normal * cosi * 2;
            if (out.d[2] < 0)
            {
                spectrum = Spectrum(0);
                return 0;
            }

            spectrum = Spectrum(1) * G(in, out);
            return 1;
        }
        else
        {
            type = 0;

            $ << "refract !" << endl;
            out = (in * IOR + normal * (cosi * IOR - cost)).scale(1);
            if (out.d[2] > 0)
            {
                spectrum = Spectrum(0);
                return 0;
            }

            spectrum = Spectrum(1) * GT(in, out, normal);
            return 1;
        }
    }
};

#endif
