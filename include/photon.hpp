#ifndef photon_hpp
#define photon_hpp

#include <utility>

#include "utils.hpp"
#include "geometry.hpp"
#include "spectrum.hpp"
#include "material.hpp"

struct Photon
{
    Ray ray;
    Spectrum spectrum;
    shared_ptr<Material> inside;
    Photon(Ray ray_, Spectrum spectrum_, shared_ptr<Material> material);

    void move(double length);
    void into(const shared_ptr<Material>& material);
    void trans(Spectrum _spectrum);
    void apply(Ray _ray);
};

#ifdef ARC_IMPLEMENTATION

Photon::Photon(Ray ray_, Spectrum spectrum_, shared_ptr<Material> material)
{
    ray = ray_;
    spectrum = spectrum_;
    inside = std::move(material);
}

void Photon::move(double length) { ray.move(length); }
void Photon::into(const shared_ptr<Material>& material)
{
    if (material != nullptr) inside = material;
}
void Photon::trans(Spectrum _spectrum) { spectrum = spectrum * _spectrum; }
void Photon::apply(Ray _ray) { ray = _ray; }

#endif
#endif /* photon_hpp */
