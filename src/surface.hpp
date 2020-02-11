#ifndef surface_hpp
#define surface_hpp

#include "utils.hpp"
#include "material.hpp"
#include "spectrum.hpp"
#include "mapping.hpp"

struct sInfo
{
    double absorb, ior, rough, diffuse;
    Spectrum emission, base, specular;
    shared_ptr<Material> inside, outside;

    sInfo ();
    sInfo (shared_ptr<Material> inside, shared_ptr<Material> outside, double rough);

    void apply(const Pixel &t);
};

class Surface
{

public:
    sInfo virtual info(const Ray &ray, Ray &normal) const;
};

class Uniform : public Surface
{
    sInfo into, outo;

public:
    Uniform (shared_ptr<Material> _inside, shared_ptr<Material> _outside, double _rough = 0);
    sInfo info(const Ray &ray, Ray &normal) const override;
};

class Textured : public Surface
{
    sInfo into, outo;
    shared_ptr<Mapping> texture;
    Trans3 T;

public:
    Textured (shared_ptr<Material> _inside, shared_ptr<Material> _outside, shared_ptr<Mapping> _texture,
        const Trans3 &_T, double _rough = 0);
    sInfo info(const Ray &ray, Ray &normal) const override;
};

#ifdef ARC_IMPLEMENTATION

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

void sInfo::apply(const Pixel &t)
{
    emission = emission * rgb(t);
    base = base * rgb(t);
    specular = specular * rgb(t);
}

sInfo Surface::info(const Ray &ray, Ray &normal) const
{
    throw "NotImplementedError";
}

Uniform::Uniform (shared_ptr<Material> _inside, shared_ptr<Material> _outside, double _rough)
{
    into = sInfo(_inside, _outside, _rough);
    outo = sInfo(_outside, _inside, _rough);
}

sInfo Uniform::info(const Ray &ray, Ray &normal) const
{
    // $ << into.absorb << " " << outo.absorb << " " << ray.d * normal.d << endl;
    if (ray.d * normal.d < 0) return into;
    else return outo;
}

Textured::Textured (shared_ptr<Material> _inside, shared_ptr<Material> _outside, shared_ptr<Mapping> _texture,
    const Trans3 &_T, double _rough)
{
    into = sInfo(_inside, _outside, _rough);
    outo = sInfo(_outside, _inside, _rough);
    texture = _texture;
    T = _T;
}

sInfo Textured::info(const Ray &ray, Ray &normal) const
{
    sInfo result;
    if (ray.d * normal.d < 0) result = into;
    else result = outo;
    Vec3 p = T.apply(normal.o);
    result.apply(texture->get(p.d[0], p.d[1]));
    return result;
}

#endif
#endif /* surface_hpp */
