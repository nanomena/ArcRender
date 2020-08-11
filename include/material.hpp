#ifndef material_hpp
#define material_hpp

#include <utility>

#include "utils.hpp"
#include "spectrum.hpp"

struct Material
{
    double absorb, ior, diffuse;
    Spectrum emission, base, specular, trans;
    string name;

    Material(
        double absorb_, Spectrum emission_,
        double ior_, double diffuse_,
        Spectrum base_, Spectrum specular_, Spectrum trans_,
        string name_
    );

    Spectrum through(double dis) const;
};

Material material_e(Spectrum emission, string name);
Material material_u(Spectrum base, Spectrum specular, Spectrum trans, double ior, double diffuse, string name);
Material material_s(double matal, Spectrum base, string name);

#ifdef ARC_IMPLEMENTATION

Material::Material(
    double absorb_, Spectrum emission_,
    double ior_, double diffuse_,
    Spectrum base_, Spectrum specular_, Spectrum trans_,
    string name_
)
{
    ior = ior_;
    absorb = absorb_;
    diffuse = diffuse_;
    base = base_;
    specular = specular_;
    emission = emission_;
    trans = trans_;
    name = std::move(name_);
}

Spectrum Material::through(double dis) const
{
    return trans ^ dis;
}
Material material_e(Spectrum emission, string name)
{
    return Material(1, emission, 0, 0, Spectrum(0), Spectrum(0), Spectrum(0), std::move(name));
}
Material material_u(Spectrum base, Spectrum specular, Spectrum trans, double ior, double diffuse, string name)
{
    return Material(0, Spectrum(0), ior, diffuse, base, specular, trans, std::move(name));
}
Material material_s(double metal, Spectrum base, string name)
{
    return material_u(base, base, Spectrum(0), 30 * metal + 1, 1 - metal, std::move(name));
}
#endif
#endif /* material_hpp */
