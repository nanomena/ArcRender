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
        return Photon(photon.stat, RD.diffuse(normal).move(EPS), photon.color * color);
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
        Ray _normal = normal;
        if (photon.ray.d * normal.d > 0) _normal.d = -normal.d;

        double p = RD.rand(0, 1);
        if ((p -= diffuse) < 0)
            return forward_diffuse(photon, _normal);
        else if ((p -= specular) < 0)
            return forward_specular(photon, _normal);
        else
            return forward_absorb(photon, _normal);
    }
};

#endif
