#ifndef surface_hpp
#define surface_hpp

#include "utils.hpp"
#include "photon.hpp"
#include "bxdf.hpp"

class Surface
{

public:
    double virtual forward(Photon &photon, int &type) const;
};

class LightSource : public Surface
{
    Spectrum spectrum;

public:
    LightSource (Spectrum light);

    double forward(Photon &photon, int &type) const;
};

class OneFaceLightSource : public Surface
{
    Spectrum spectrum;

public:
    OneFaceLightSource (Spectrum light);

    double forward(Photon &photon, int &type) const;
};

class MetalSurface : public Surface
{
    shared_ptr<BxDF> subsurface;

public:
    MetalSurface (Spectrum _albedo, double _rough, double _metal);

    double forward(Photon &photon, int &type) const;
};


class TransSurface : public Surface
{
    shared_ptr<Material> inside, outside;
    shared_ptr<BxDF> into, outo;

public:
    TransSurface (shared_ptr<Material> _inside, double _rough, shared_ptr<Material> _outside = air);

    double forward(Photon &photon, int &type) const;
};

#ifdef library

double Surface::forward(Photon &photon, int &type) const
{
    throw "NotImplementedError";
}


LightSource::LightSource (Spectrum light)
{
    spectrum = light;
}
double LightSource::forward(Photon &photon, int &type) const
{
    type = -1;
    photon.trans(spectrum);
    return 1;
}

OneFaceLightSource::OneFaceLightSource (Spectrum light)
{
    spectrum = light;
}

double OneFaceLightSource::forward(Photon &photon, int &type) const
{
    type = -1;
    if (photon.ray.d.d[2] > 0)
        photon.trans(Spectrum(0));
    else
        photon.trans(spectrum);
    return 1;
}

MetalSurface::MetalSurface (Spectrum _albedo, double _rough, double _metal)
{
    subsurface = make_shared<Metal>(_albedo, _rough, _metal);
}
double MetalSurface::forward(Photon &photon, int &type) const
{
    if (photon.ray.d.d[2] > 0) return 0;
    Vec3 out;
    Spectrum spectrum;
    double weight = subsurface->through(photon.ray.d, out, spectrum, type);
    photon.apply(Ray(Vec3(), out));
    photon.trans(spectrum);
    return weight;
}

TransSurface::TransSurface (shared_ptr<Material> _inside, double _rough, shared_ptr<Material> _outside)
{
    inside = _inside;
    outside = _outside;
    into = make_shared<Trans>(outside->IOR / inside->IOR, _rough);
    outo = make_shared<Trans>(inside->IOR / outside->IOR, _rough);
}
double TransSurface::forward(Photon &photon, int &type) const
{
    Vec3 in = photon.ray.d, out;
    Spectrum spectrum;
    double weight;
    if (photon.ray.d.d[2] < 0)
    {
        weight = into->through(in, out, spectrum, type);
        if (out.d[2] < 0) photon.into(inside);
        photon.apply(Ray(Vec3(), out));
        photon.trans(spectrum);
    }
    else
    {
        in.d[2] *= -1;
        weight = outo->through(in, out, spectrum, type);
        out.d[2] *= -1;
        if (out.d[2] > 0) photon.into(outside);
        photon.apply(Ray(Vec3(), out));
        photon.trans(spectrum);
    }
    return weight;
}

#endif
#endif /* surface_hpp */
