#ifndef surface_hpp
#define surface_hpp

#include "utils.hpp"
#include "material.hpp"
#include "spectrum.hpp"

struct sInfo
{
    double absorb, ior, rough, diffuse;
    Spectrum emission, base, specular;
    shared_ptr<Material> inside, outside;

    sInfo ();
    sInfo (shared_ptr<Material> inside, shared_ptr<Material> outside, double rough);
};

class Surface
{

public:
    sInfo virtual info(const Ray &ray, const Ray &normal) const;
};

class Uniform : public Surface
{
    sInfo into, outo;

public:
    Uniform (shared_ptr<Material> _inside, shared_ptr<Material> _outside, double _rough = 0);
    sInfo info(const Ray &ray, const Ray &normal) const override;
};

#ifndef library

sInfo::sInfo () {}
sInfo::sInfo (shared_ptr<Material> _inside, shared_ptr<Material> _outside, double _rough)
{
    inside = _inside;
    outside = _outside;
    ior = outside->ior / inside->ior;
    rough = _rough;
    absorb = inside->absorb;
    diffuse = inside->diffuse;
    base = inside->base;
    specular = inside->specular;
    emission = inside->emission;
}

sInfo Surface::info(const Ray &ray, const Ray &normal) const
{
    throw "NotImplementedError";
}

Uniform::Uniform (shared_ptr<Material> _inside, shared_ptr<Material> _outside, double _rough)
{
    into = sInfo(_inside, _outside, _rough);
    outo = sInfo(_outside, _inside, _rough);
}

sInfo Uniform::info(const Ray &ray, const Ray &normal) const
{
    // $ << into.absorb << " " << outo.absorb << " " << ray.d * normal.d << endl;
    if (ray.d * normal.d < 0) return into;
    else return outo;
}

#endif
#endif /* surface_hpp */
