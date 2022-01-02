#ifndef object_hpp
#define object_hpp

#include "utils.hpp"
#include "vecmath.hpp"
#include "bxdf.hpp"
#include "light.hpp"
#include "medium.hpp"

class Medium;

class Object {
public:
    Object() = default;

    virtual bool isLight() const = 0;

    virtual void sampleSurface(Vec3 &pos, double &pdf, Sampler &RNG) const = 0;
    virtual double evaluateSurfaceImportance(const Vec3 &pos) const = 0;

    virtual Spectrum evaluateBxDF(const Ray &v, const Ray &l) const = 0;
    virtual Spectrum sampleBxDF(const Ray &v, const Vec3 &pos, Ray &l, shared_ptr<Medium> &medium, Sampler &RNG) const = 0;
    virtual double evaluateBxDFImportance(const Ray &v, const Ray &l) const = 0;

    virtual Spectrum evaluateLight(const Ray &v, const Vec3 &pos) const = 0;
    virtual Spectrum evaluateLightBack(const Ray &vB) const = 0;
    virtual Spectrum sampleLight(Ray &lB, shared_ptr<Medium> &medium, Sampler &RNG) const = 0;
    virtual double evaluateLightImportance(const Ray &lB) const = 0;

    void setIdentifier(const string &Name);
    string name;

protected:
};

#ifdef ARC_IMPLEMENTATION

void Object::setIdentifier(const string &Name) {
    this->name = Name;
}

#endif
#endif /* object_hpp */