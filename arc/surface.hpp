#ifndef SURFACE
#define SURFACE
#include "photon.hpp"
#include "sampler.hpp"

class Surface
{

public:
    Surface () {}
    Photon virtual forward(const Photon &photon, const Ray &normal) const { throw "NotImplementedError"; }
};

class LightSource : public Surface
{
    Color color;

public:
    LightSource (Color _color)
    {
        color = _color;
    }

    Photon forward(const Photon &photon, const Ray &normal) const
    {
        $ << "shine!" << endl;
        return Photon(photon.stat | 2, normal, photon.color * color);
    }
};

class NaiveSurface : public Surface
{
    Color color;
    double diffuse, specular;

public:
    NaiveSurface (Color _color, double _diffuse, double _specular)
    {
        color = _color;
        diffuse = _diffuse;
        specular = _specular;
    }

    Photon forward_diffuse(const Photon &photon, const Ray &normal) const
    {
        $ << "diffuse!" << endl;
        Ray _normal = normal;
        if (photon.ray.d * normal.d > 0) _normal.d = -normal.d;
        return Photon(photon.stat, RD.diffuse(_normal).move(EPS), photon.color * color);
    }
    Photon forward_specular(const Photon &photon, const Ray &normal) const
    {
        $ << "specular!" << endl;
        return Photon(photon.stat, photon.ray.reflect(normal).move(EPS), photon.color * color);
    }
    Photon forward_absorb(const Photon &photon, const Ray &normal) const
    {
        $ << "absorb!" << endl;
        return Photon(photon.stat | 3, normal, Color(0));
    }
    Photon forward(const Photon &photon, const Ray &normal) const
    {
        double p = RD.rand(0, 1);
        if ((p -= diffuse) < 0)
            return forward_diffuse(photon, normal);
        else if ((p -= specular) < 0)
            return forward_specular(photon, normal);
        else
            return forward_absorb(photon, normal);
    }
};

class InsulatedHitEvent
{
    Ray i, n;
    double cosi, cost, eta;

public:
    double fresnel()
    {
        double rs = (eta * cosi - cost) / (eta * cost + cosi);
        double rp = (eta * cost - cosi) / (eta * cosi + cost);
        return (rs * rs + rp * rp) * 0.5;
    }

    InsulatedHitEvent(Ray _i, Ray _n, double _eta)
    {
        i = _i, n = _n; eta = _eta;
    }
    double refl()
    {
        cosi = -i.d * n.d;
        if (sqrt(1 - cosi * cosi) * eta > 1) return 1;
        else
        {
            cost = sqrt(1 - (1 - cosi * cosi) * eta * eta);
            return fresnel();
        }
    }
    Ray reflect()
    {
        return Ray(n.o, i.d + n.d * (cosi * 2));
    }
    Ray refract()
    {
        return Ray(n.o, i.d * eta + n.d * (cosi * eta - cost));
    }
};

class SmoothInsulatedSurface : public Surface
{
    Color color; double eta;

public:
    SmoothInsulatedSurface (Color _color, double _eta)
    {
        color = _color;
        eta = _eta;
    }

    Photon forward(const Photon &photon, const Ray &normal) const
    {
        // $ << photon.ray.d.norm2() << " " << normal.d.norm2() << endl;
        double cosi = photon.ray.d * normal.d;
        InsulatedHitEvent hit_event(photon.ray, (cosi > 0 ? -normal : normal), (cosi > 0 ? eta : 1 / eta));

        double p = RD.rand(0, 1);
        if ((p -= hit_event.refl()) < 0)
        {
            $ << "reflect!" << endl;
            return Photon(photon.stat, hit_event.reflect().move(EPS), photon.color);
        }
        else
        {
            $ << "refract!" << endl;
            double dis = (cosi > 0 ? 0 : (photon.ray.d - normal.d).norm());
            return Photon(photon.stat, hit_event.refract().move(EPS), photon.color * (color ^ dis));
        }
    }
};

#endif
