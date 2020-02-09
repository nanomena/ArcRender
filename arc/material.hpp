#ifndef MATERIAL
#define MATERIAL
#include "arc.hpp"
#include "spectrum.hpp"

struct Material
{
    Spectrum trans;
    double IOR;
    string name;

    Material (double _IOR, Spectrum _trans, string _name)
    {
        IOR = _IOR;
        trans = _trans;
        name = _name;
    }

    Spectrum through(double dis)
    {
        return trans ^ dis;
    }
};

shared_ptr<Material> air = make_shared<Material>(1, Spectrum(1), "air");

#endif
