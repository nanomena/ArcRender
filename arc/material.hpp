#ifndef material_hpp
#define material_hpp

#include "utils.hpp"
#include "spectrum.hpp"

struct Material
{
    double absorb, ior, metal, diffuse;
    Spectrum emission, base, specular, trans;
    string name;

    Material (double _absorb, Spectrum _emission, \
        double _ior, double _metal, double _diffuse, \
        Spectrum _base, Spectrum _specular, Spectrum _trans, \
        string _name);

    Spectrum through(double dis) const;
};

#ifndef library

Material::Material (double _absorb, Spectrum _emission, \
    double _ior, double _metal, double _diffuse, \
    Spectrum _base, Spectrum _specular, Spectrum _trans, \
    string _name)
{
    ior = _ior;
    metal = _metal;
    absorb = _absorb;
    diffuse = _diffuse;
    base = _base;
    specular = _specular;
    emission = _emission;
    trans = _trans;
    name = _name;
}

Spectrum Material::through(double dis) const
{
    return trans ^ dis;
}

#endif
#endif /* material_hpp */
