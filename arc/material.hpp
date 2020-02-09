#ifndef material_hpp
#define material_hpp

#include "utils.hpp"
#include "spectrum.hpp"

struct Material
{
    Spectrum trans;
    double IOR;
    string name;

    Material (double _IOR, Spectrum _trans, string _name);
    Spectrum through(double dis);
};

static shared_ptr<Material> air = make_shared<Material>(1, Spectrum(1), "air");

#ifdef library

Material::Material (double _IOR, Spectrum _trans, std::string _name)
{
    IOR = _IOR;
    trans = _trans;
    name = _name;
}

Spectrum Material::through(double dis)
{
    return trans ^ dis;
}

#endif
#endif /* material_hpp */
