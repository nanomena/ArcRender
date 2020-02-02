#ifndef PHOTON
#define PHOTON
#include "geo.hpp"
#include "spectrum.hpp"

struct Photon
{
    Vec3 o, d;
    char stat; Ray ray; Spectrum spectrum;
    Photon (Ray _ray, Spectrum _spectrum = Spectrum(1), char _stat = 0)
    {
        stat = _stat;
        ray = _ray;
        spectrum = _spectrum;
    }

    void move(double length) { ray.move(length); }
    void trans(Spectrum _spectrum) { spectrum = spectrum * _spectrum; }
    void apply(Ray _ray) { ray = _ray; }
    void visit(int tag) { stat |= 1 << tag; }
    int is_visit(int tag) const { return stat & (1 << tag); }
};


#endif
