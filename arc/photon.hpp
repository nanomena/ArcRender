#ifndef PHOTON
#define PHOTON
#include "geo.hpp"
#include "spectrum.hpp"
#include "material.hpp"

struct Photon
{
    Ray ray; Spectrum spectrum;
    shared_ptr<Material> inside;
    Photon (Ray _ray, Spectrum _spectrum = Spectrum(1), shared_ptr<Material> material = air)
    {
        ray = _ray;
        spectrum = _spectrum;
        inside = material;
    }

    void move(double length) { ray.move(length); }
    void into(shared_ptr<Material> material) { inside = material; }
    void trans(Spectrum _spectrum) { spectrum = spectrum * _spectrum; }
    void apply(Ray _ray) { ray = _ray; }
};


#endif
