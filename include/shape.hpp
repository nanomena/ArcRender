#ifndef shape_hpp
#define shape_hpp

#include "utils.hpp"
#include "vecmath.hpp"
#include "bxdf.hpp"
#include "light.hpp"
#include "medium.hpp"
#include "object.hpp"

class Shape : public Object {
public:
    Shape(
        const shared_ptr<BxDF> &bxdf, const shared_ptr<Light> &light,
        const shared_ptr<Medium> &inside, const shared_ptr<Medium> &outside
    );

    bool isLight() const override;

    virtual bool intersect(const Ray &ray, double &t) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Vec3 normal(const Vec3 &inter) const {
        throw invalid_argument("NotImplementedError");
    } // norm should be unitary
    void sampleSurface(Vec3 &pos, double &pdf, Sampler &RNG) const override {
        throw invalid_argument("NotImplementedError");
    }
    double evaluateSurfaceImportance(const Vec3 &pos) const override {
        throw invalid_argument("NotImplementedError");
    }
    shared_ptr<Medium> getMedium(const Ray &test) const;

    Spectrum evaluateBxDF(const Ray &v, const Ray &l) const override;
    Spectrum sampleBxDF(const Ray &v, const Vec3 &pos, Ray &l, shared_ptr<Medium> &medium, Sampler &RNG) const override;
    double evaluateBxDFImportance(const Ray &v, const Ray &l) const override;

    Spectrum evaluateLight(const Ray &v, const Vec3 &pos) const override;
    Spectrum evaluateLightBack(const Ray &vB) const override;
    Spectrum sampleLight(Ray &lB, shared_ptr<Medium> &medium, Sampler &RNG) const override;
    double evaluateLightImportance(const Ray &lB) const override;

    virtual Box3 outline() const;

protected:
    virtual shared_ptr<BxDF> getBxDF(const Vec3 &pos) const;
    virtual shared_ptr<Light> getLight(const Vec3 &pos) const;

    shared_ptr<BxDF> bxdf;
    shared_ptr<Light> light;
    shared_ptr<Medium> inside, outside;
    Box3 box;
};

#ifdef ARC_IMPLEMENTATION

Shape::Shape(
    const shared_ptr<BxDF> &bxdf, const shared_ptr<Light> &light,
    const shared_ptr<Medium> &inside, const shared_ptr<Medium> &outside
) : bxdf(bxdf), light(light), inside(inside), outside(outside) {}

shared_ptr<BxDF> Shape::getBxDF(const Vec3 &pos) const {
    return bxdf;
}
shared_ptr<Light> Shape::getLight(const Vec3 &pos) const {
    return light;
}
bool Shape::isLight() const {
    return light != nullptr;
}
Box3 Shape::outline() const {
    return box;
}

shared_ptr<Medium> Shape::getMedium(const Ray &test) const {
    Vec3 n = normal(test.o);
    return (n * test.d > 0 ? outside : inside);
}

Spectrum Shape::evaluateBxDF(const Ray &v, const Ray &l) const {
    assert(abs(1 - v.d.length()) < EPS);
    assert(abs(1 - l.d.length()) < EPS);
    Vec3 n = normal(l.o);
    return getBxDF(l.o)->evaluate(n, -v.d, l.d) * abs(l.d * n);
}

double Shape::evaluateBxDFImportance(const Ray &v, const Ray &l) const {
    Vec3 n = normal(l.o);
    return getBxDF(l.o)->evaluateImportance(n, -v.d, l.d) * abs(l.d * n);
}

Spectrum Shape::sampleBxDF(const Ray &v, const Vec3 &pos, Ray &l, shared_ptr<Medium> &medium, Sampler &RNG) const {
    assert(abs(1 - v.d.length()) < EPS);
    l.o = pos;
    Vec3 n = normal(l.o);
    Spectrum s = getBxDF(l.o)->sample(n, -v.d, l.d, RNG);
//    if (n * v.d < 0) assert(medium == outside); else assert(medium == inside);
    medium = getMedium(l);
    return s * abs(l.d * n);
}

Spectrum Shape::evaluateLight(const Ray &v, const Vec3 &pos) const {
    if (!isLight()) return Spectrum(0);
    assert(abs(1 - v.d.length()) < EPS);
    Vec3 n = normal(pos);
    return getLight(pos)->evaluate(n, -v.d);
}

Spectrum Shape::evaluateLightBack(const Ray &vB) const {
    Vec3 n = normal(vB.o);
    return getLight(vB.o)->evaluate(n, vB.d) * abs(vB.d * n);
}

Spectrum Shape::sampleLight(Ray &lB, shared_ptr<Medium> &medium, Sampler &RNG) const {
    if (!isLight()) return Spectrum(0);
    double pdf;
    sampleSurface(lB.o, pdf, RNG);
    Vec3 n = normal(lB.o);
    Spectrum s = getLight(lB.o)->sample(n, lB.d, RNG) / pdf;
    medium = getMedium(lB);
    return s * abs(lB.d * n);
}

double Shape::evaluateLightImportance(const Ray &lB) const {
    if (!isLight()) return 0;
    Vec3 n = normal(lB.o);
    return getLight(lB.o)->evaluateImportance(n, lB.d);
}

#endif
#endif /* shape_hpp */