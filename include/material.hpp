#ifndef material_hpp
#define material_hpp

#include "utils.hpp"
#include "spectrum.hpp"

struct Material
{
    double absorb, ior, diffuse;
    Spect emission, base, specular, trans;
    string name;

    Material(
        double absorb_, Spect emission_,
        double ior_, double diffuse_,
        Spect base_, Spect specular_, Spect trans_,
        string name_
    );

    Spect through(double dis) const;
};

Material material_e(Spect emission, string name);
Material material_u(Spect base, Spect specular, Spect trans, double ior, double diffuse, string name);
Material material_s(double metal, Spect base, string name);

#ifdef ARC_IMPLEMENTATION

Material::Material(
    double absorb_, Spect emission_,
    double ior_, double diffuse_,
    Spect base_, Spect specular_, Spect trans_,
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

Spect Material::through(double dis) const
{
    return trans ^ dis;
}
Material material_e(Spect emission, string name)
{
    return Material(1, emission, 0, 0, Spect(0), Spect(0), Spect(0), std::move(name));
}
Material material_u(Spect base, Spect specular, Spect trans, double ior, double diffuse, string name)
{
    return Material(0, Spect(0), ior, diffuse, base, specular, trans, std::move(name));
}
Material material_s(double metal, Spect base, string name)
{
    return material_u(base, base, Spect(0), 30 * metal + 1, 1 - metal, std::move(name));
}
#endif
#endif /* material_hpp */
