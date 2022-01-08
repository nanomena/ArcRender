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

    virtual void sampleSurface(Vec3 &pos, Vec2 &texPos, double &pdf, Sampler &RNG) const = 0;
    virtual double evaluateSurfaceImportance(const Vec3 &pos, const Vec2 &texPos) const = 0;

    virtual Spectrum evaluateBxDF(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, const Vec3 &l) const = 0;
    virtual Spectrum sampleBxDF(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, Vec3 &l, const Medium *&medium, Sampler &RNG) const = 0;
    virtual double evaluateBxDFImportance(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, const Vec3 &l) const = 0;

    virtual Spectrum evaluateLight(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos) const = 0;
    virtual Spectrum evaluateLightBack(const Vec3 &lB, const Vec3 &pos, const Vec2 &texPos) const = 0;
    virtual Spectrum sampleLight(Vec3 &lB, Vec3 &pos, Vec2 &texPos, const Medium *&medium, Sampler &RNG) const = 0;
    virtual double evaluateLightImportance(const Vec3 &lB, const Vec3 &pos, const Vec2 &texPos) const = 0;

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