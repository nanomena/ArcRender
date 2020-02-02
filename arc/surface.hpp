#ifndef SURFACE
#define SURFACE
#include "photon.hpp"
#include "sampler.hpp"
#include "bxdf.hpp"

class Surface
{

public:
    Surface () {}
    double virtual forward(Photon &photon) const { throw "NotImplementedError"; }
};

class LightSource : public Surface
{
    Spectrum spectrum;

public:
    LightSource (Spectrum light)
    {
        spectrum = light;
    }

    double forward(Photon &photon) const
    {
        $ << "shine!" << endl;
        photon.trans(spectrum);
        photon.visit(1);
        return 1;
    }
};

class BRDFSurface : public Surface, public BRDF
{

public:
    BRDFSurface (Spectrum _albedo, double _rough, double _metal)
        : BRDF(_albedo, _rough, _metal) {}

    double forward(Photon &photon) const
    {
        Vec3 out;
        Spectrum spectrum;
        double weight = through(photon.ray.d, out, spectrum);
        photon.apply(Ray(Vec3(), out));
        photon.trans(spectrum);
        return weight;
    }
};



#endif
