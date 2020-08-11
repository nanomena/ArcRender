#ifndef surface_hpp
#define surface_hpp

#include <utility>

#include "utils.hpp"
#include "material.hpp"
#include "spectrum.hpp"
#include "mapping.hpp"

struct sInfo
{
    double absorb, ior, rough, diffuse;
    Spectrum emission, base, specular;
    shared_ptr<Material> inside, outside;

    sInfo();
    sInfo(shared_ptr<Material> inside_, shared_ptr<Material> outside_, double rough_);
    sInfo(const shared_ptr<Material> &material, double rough_);

    void apply(const Pixel &t);
};

class Surface
{

public:
    virtual sInfo info(const Ray &ray, Ray &normal) const;
};

class Solid : public Surface
{
    sInfo into, outo;
    shared_ptr<Mapping> texture;
    Trans3 T;

public:
    Solid(const shared_ptr<Material> &inside_, const shared_ptr<Material> &outside_, double rough_ = 0);
    Solid(
        const shared_ptr<Material> &inside_, const shared_ptr<Material> &outside_, shared_ptr<Mapping> texture_,
        const Trans3 &T_, double rough_ = 0
    );

    sInfo info(const Ray &ray, Ray &normal) const override;
};

class Thin : public Surface
{
    sInfo surface;
    shared_ptr<Mapping> texture;
    Trans3 T;

public:
    explicit Thin(const shared_ptr<Material> &material, double rough_ = 0);
    Thin(
        const shared_ptr<Material> &material, shared_ptr<Mapping> texture_,
        const Trans3 &T_, double rough = 0
    );

    sInfo info(const Ray &ray, Ray &normal) const override;
};

#ifdef ARC_IMPLEMENTATION

sInfo::sInfo() = default;
sInfo::sInfo(shared_ptr<Material> inside_, shared_ptr<Material> outside_, double rough_)
{
    inside = std::move(inside_);
    outside = std::move(outside_);
    ior = outside->ior / inside->ior;
    rough = rough_;
    absorb = inside->absorb;
    diffuse = inside->diffuse;
    base = inside->base;
    specular = inside->specular;
    emission = inside->emission;
}

sInfo::sInfo(const shared_ptr<Material> &material, double rough_)
{
    inside = outside = nullptr;
    ior = 1 / material->ior;
    rough = rough_ * (ior - 1);
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
    throw invalid_argument("NotImplementedError");
}

Solid::Solid(const shared_ptr<Material> &inside_, const shared_ptr<Material> &outside_, double rough_)
{
    into = sInfo(inside_, outside_, rough_);
    outo = sInfo(outside_, inside_, rough_);
    texture = nullptr;
}

Solid::Solid(
    const shared_ptr<Material> &inside_, const shared_ptr<Material> &outside_, shared_ptr<Mapping> texture_,
    const Trans3 &T_, double rough_
)
{
    into = sInfo(inside_, outside_, rough_);
    outo = sInfo(outside_, inside_, rough_);
    texture = std::move(texture_);
    T = T_;
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

Thin::Thin(const shared_ptr<Material> &material, double rough_)
{
    surface = sInfo(material, rough_);
    texture = nullptr;
}

Thin::Thin(
    const shared_ptr<Material> &material, shared_ptr<Mapping> texture_,
    const Trans3 &T_, double rough
)
{
    surface = sInfo(material, rough);
    texture = std::move(texture_);
    T = T_;
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
