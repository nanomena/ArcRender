#ifndef SURFACE
#define SURFACE
#include "photon.hpp"
#include "sampler.hpp"
#include "bxdf.hpp"
#include <cassert>

class Surface
{

public:
    Surface () {}
    double virtual forward(Photon &photon, int &type) const { throw "NotImplementedError"; }
};

class LightSource : public Surface
{
    Spectrum spectrum;

public:
    LightSource (Spectrum light)
    {
        spectrum = light;
    }

    double forward(Photon &photon, int &type) const
    {
        type = -1;

        $ << "shine!" << endl;
        photon.trans(spectrum);
        return 1;
    }
};

class OneFaceLightSource : public Surface
{
    Spectrum spectrum;

public:
    OneFaceLightSource (Spectrum light)
    {
        spectrum = light;
    }

    double forward(Photon &photon, int &type) const
    {
        type = -1;

        $ << "shine!" << endl;
        if (photon.ray.d.d[2] > 0)
            photon.trans(Spectrum(0));
        else
            photon.trans(spectrum);
        return 1;
    }
};

class MetalSurface : public Surface
{
    shared_ptr<BxDF> subsurface;

public:
    MetalSurface (Spectrum _albedo, double _rough, double _metal)
    {
        subsurface = make_shared<Metal>(_albedo, _rough, _metal);
    }

    double forward(Photon &photon, int &type) const
    {
        if (photon.ray.d.d[2] > 0) return 0;
        Vec3 out;
        Spectrum spectrum;
        double weight = subsurface->through(photon.ray.d, out, spectrum, type);
        photon.apply(Ray(Vec3(), out));
        photon.trans(spectrum);
        $ << "WEIGHT  " << weight << endl;
        return weight;
    }
};


class TransSurface : public Surface
{
    shared_ptr<Material> inside, outside;
    shared_ptr<BxDF> into, outo;

public:
    TransSurface (shared_ptr<Material> _inside, double _rough, shared_ptr<Material> _outside = air)
    {
        inside = _inside;
        outside = _outside;
        into = make_shared<Trans>(outside->IOR / inside->IOR, _rough);
        outo = make_shared<Trans>(inside->IOR / outside->IOR, _rough);
    }

    double forward(Photon &photon, int &type) const
    {
        Vec3 in = photon.ray.d, out;
        Spectrum spectrum;
        double weight;
        if (photon.ray.d.d[2] < 0)
        {
            weight = into->through(photon.ray.d, out, spectrum, type);
            photon.apply(Ray(Vec3(), out));
            if (out.d[2] < 0)
            {
                // $ << "in" << endl;
                photon.into(inside);
            }
            photon.trans(spectrum);
        }
        else
        {
            photon.ray.d.d[2] *= -1;
            weight = outo->through(photon.ray.d, out, spectrum, type);
            out.d[2] *= -1;
            photon.apply(Ray(Vec3(), out));
            if (out.d[2] > 0)
            {
                // $ << "out" << endl;
                photon.into(outside);
            }
            photon.trans(spectrum);
        }
        $ << "WEIGHT  " << weight << endl;
        return weight;
    }
};




#endif
