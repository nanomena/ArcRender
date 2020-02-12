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
    sInfo (shared_ptr<Material> inside, shared_ptr<Material> outside, double _rough);
    sInfo (shared_ptr<Material> material, double _rough);

    void apply(const Pixel &t);
};

class Surface
{

public:
    sInfo virtual info(const Ray &ray, Ray &normal) const;
};

class Solid : public Surface
{
    sInfo into, outo;
    shared_ptr<Mapping> texture;
    Trans3 T;

public:
    Solid (shared_ptr<Material> _inside, shared_ptr<Material> _outside, double _rough = 0);
    Solid (shared_ptr<Material> _inside, shared_ptr<Material> _outside, shared_ptr<Mapping> _texture,
        const Trans3 &_T, double _rough = 0);

    sInfo info(const Ray &ray, Ray &normal) const override;
};

class Thin : public Surface
{
    sInfo surface;
    shared_ptr<Mapping> texture;
    Trans3 T;

public:
    Thin (shared_ptr<Material> material, double _rough = 0);
    Thin (shared_ptr<Material> material, shared_ptr<Mapping> _texture,
        const Trans3 &_T, double rough = 0);

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

sInfo::sInfo (shared_ptr<Material> material, double _rough)
{
    inside = outside = nullptr;
    ior = 1 / material->ior;
    rough = _rough * (ior - 1);
    absorb = material->absorb;
    diffuse = material->diffuse;
    base = material->base;
    specular = material->specular;
    emission = material->emission;
}


void sInfo::apply(const Pixel &t)
{
    emission = emission * rgb(t);
    base = base * rgb(t);
    specular = specular * rgb(t);
    diffuse *= (1 - t.a);
}

sInfo Surface::info(const Ray &ray, Ray &normal) const
{
    throw "NotImplementedError";
}

Solid::Solid (shared_ptr<Material> _inside, shared_ptr<Material> _outside, double _rough)
{
    into = sInfo(_inside, _outside, _rough);
    outo = sInfo(_outside, _inside, _rough);
    texture = nullptr;
}

Solid::Solid (shared_ptr<Material> _inside, shared_ptr<Material> _outside, shared_ptr<Mapping> _texture,
    const Trans3 &_T, double _rough)
{
    into = sInfo(_inside, _outside, _rough);
    outo = sInfo(_outside, _inside, _rough);
    texture = _texture;
    T = _T;
}

sInfo Solid::info(const Ray &ray, Ray &normal) const
{
    sInfo result;
    if (ray.d * normal.d < 0) result = into;
    else result = outo;
    if (texture != nullptr)
    {
        Vec3 p = T.apply(normal.o);
        result.apply(texture->get(p.d[0], p.d[1]));
    }
    return result;
}

Thin::Thin (shared_ptr<Material> material, double rough)
{
    surface = sInfo(material, rough);
    texture = nullptr;
}

Thin::Thin (shared_ptr<Material> material, shared_ptr<Mapping> _texture,
    const Trans3 &_T, double rough)
{
    surface = sInfo(material, rough);
    texture = _texture;
    T = _T;
}

sInfo Thin::info(const Ray &ray, Ray &normal) const
{
    sInfo result = surface;
    if (texture != nullptr)
    {
        Vec3 p = T.apply(normal.o);
        result.apply(texture->get(p.d[0], p.d[1]));
    }
    return result;
}

#endif
#endif /* surface_hpp */
