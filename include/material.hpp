#ifndef material_hpp
#define material_hpp

#include "utils.hpp"
#include "spectrum.hpp"

struct Material
{
    double absorb, ior, diffuse;
    Spectrum emission, base, specular, trans;
    string name;

    Material (double _absorb, Spectrum _emission, \
        double _ior, double _diffuse, \
        Spectrum _base, Spectrum _specular, Spectrum _trans, \
        string _name);

    Spectrum through(double dis) const;
};

Material material_e(Spectrum emission, string name);
Material material_u(Spectrum base, Spectrum specular, Spectrum trans, double ior, double diffuse, string name);
Material material_s(double matal, Spectrum base, string name);

#ifdef ARC_IMPLEMENTATION

Material::Material (double _absorb, Spectrum _emission, \
    double _ior, double _diffuse, \
    Spectrum _base, Spectrum _specular, Spectrum _trans, \
    string _name)
{
    ior = _ior;
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
Material material_e(Spectrum emission, string name)
{
    return Material(1, emission, 0, 0, Spectrum(0), Spectrum(0), Spectrum(0), name);
}
Material material_u(Spectrum base, Spectrum specular, Spectrum trans, double ior, double diffuse, string name)
{
    return Material(0, Spectrum(0), ior, diffuse, base, specular, trans, name);
}
Material material_s(double metal, Spectrum base, string name)
{
    return material_u(base, base, Spectrum(0), 30 * metal + 1, 1 - metal, name);
}
#endif
#endif /* material_hpp */
