#ifndef photon_hpp
#define photon_hpp

#include "utils.hpp"
#include "geometry.hpp"
#include "spectrum.hpp"
#include "material.hpp"

struct Photon
{
    Ray ray; Spectrum spectrum;
    shared_ptr<Material> inside;
    Photon (Ray _ray, Spectrum _spectrum, shared_ptr<Material> material);

    void move(double length);
    void into(shared_ptr<Material> material);
    void trans(Spectrum _spectrum);
    void apply(Ray _ray);
};

#ifdef ARC_IMPLEMENTATION

Photon::Photon (Ray _ray, Spectrum _spectrum, shared_ptr<Material> material)
{
    ray = _ray;
    spectrum = _spectrum;
    inside = material;
}

void Photon::move(double length) { ray.move(length); }
void Photon::into(shared_ptr<Material> material)
{
    if (material != nullptr) inside = material;
}
void Photon::trans(Spectrum _spectrum) { spectrum = spectrum * _spectrum; }
void Photon::apply(Ray _ray) { ray = _ray; }


#endif
#endif /* photon_hpp */
