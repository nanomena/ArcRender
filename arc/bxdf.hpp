#ifndef BXDF
#define BXDF
#include "sampler.hpp"

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
        // return 1;

        double k = (rough + 1) * (rough + 1) / 8;
        double g_in = -in.d[2] / (-in.d[2] * (1 - k) + k);
        double g_out = out.d[2] / (out.d[2] * (1 - k) + k);
        double g = g_in * g_out / (-in.d[2] * out.d[2]);
        return g / 4;
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

            spectrum = F(in, normal) / prob;

            return G(in, out);
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
        double rs = (IOR * cosi - cost) / (IOR * cost + cosi);
        double rp = (IOR * cost - cosi) / (IOR * cosi + cost);
        return (rs * rs + rp * rp) * 0.5;
    }

    double D(const Vec3 &normal) const
    {
        double nz2 = normal.d[2] * normal.d[2];
        return alpha2 / (pi * (nz2 * (alpha2 - 1) + 1) * (nz2 * (alpha2 - 1) + 1));
    }

    double G(const Vec3 &in, const Vec3 &out) const
    {
        return 1;

        double k = (rough + 1) * (rough + 1) / 8;
        double g_in = -in.d[2] / (-in.d[2] * (1 - k) + k);
        double g_out = out.d[2] / (out.d[2] * (1 - k) + k);
        double g = g_in * g_out / (-in.d[2] * out.d[2]);
        return g / 4;
    }

    double GT(const Vec3 &in, const Vec3 &out, const Vec3 &normal) const
    {
        return 1;

        double k = (rough + 1) * (rough + 1) / 8;
        double g_in = -in.d[2] / (-in.d[2] * (1 - k) + k);
        double g_out = -out.d[2] / (-out.d[2] * (1 - k) + k);
        double g = g_in * g_out / (-in.d[2] * -out.d[2]);
        return g;

        double c_in = (in * normal) * (IOR < 1 ? -1 : 1);
        double c_out = (out * normal) * (IOR < 1 ? 1 : -1);
        // cerr << c_in << " " << c_out << endl;
        return g * abs(c_in) * abs(c_out) / ((c_in + c_out / IOR) * (c_in + c_out / IOR));
    }

    double through(const Vec3 &in, Vec3 &out, Spectrum &spectrum, int &type) const
    {
        $ << "Trans " << in << endl;
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

            spectrum = Spectrum(1);
            double weight = G(in, out);
            // cerr << "L " << weight << endl;
            return weight;
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

            spectrum = Spectrum(1);
            double weight = GT(in, out, normal);
            // cerr << "R " << weight << endl;
            return weight;
        }
    }
};

#endif
