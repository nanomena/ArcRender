#ifndef shape_hpp
#define shape_hpp

#include "utils.hpp"
#include "vecmath.hpp"
#include "bxdf.hpp"
#include "light.hpp"
#include "medium.hpp"

class Shape {
public:
    Shape(
        const shared_ptr<BxDF> &bxdf, const shared_ptr<Light> &light,
        const shared_ptr<Medium> &inside, const shared_ptr<Medium> &outside
    );

    virtual bool isLight() const;

    virtual bool intersect(const Ray &ray, double &t) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Vec3 normal(const Vec3 &inter) const {
        throw invalid_argument("NotImplementedError");
    } // norm should be unitary

    Spectrum evaluateBxDF(const Ray &v, const Ray &l) const;
    Spectrum evaluateBxDF(const Ray &v, const Ray &l, double dis) const;
    Spectrum sampleBxDF(const Ray &v, Ray &l, shared_ptr<Medium> &medium, Sampler &RNG) const;

    Spectrum evaluateLight(const Ray &v) const;
    Spectrum sampleLight(const Vec3 &ref, Ray &v, double &t, Sampler &RNG) const;
    Spectrum sampleLight(Ray &vB, shared_ptr<Medium> &medium, Sampler &RNG) const;
    bool glossy(const Vec3 &pos) const;

    virtual Box3 outline() const;

    void setIdentifier(const string &Name);
    string name;

protected:
    virtual shared_ptr<BxDF> getBxDF(const Vec3 &pos) const;
    virtual shared_ptr<Light> getLight(const Vec3 &pos) const;
    virtual void sample(Vec3 &pos, double &pdf, Sampler &RNG) const {
        throw invalid_argument("NotImplementedError");
    }

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

bool Shape::glossy(const Vec3 &pos) const {
    return getBxDF(pos)->glossy();
}

Spectrum Shape::evaluateBxDF(const Ray &v, const Ray &l) const {
    assert(abs(1 - v.d.length()) < EPS);
    assert(abs(1 - l.d.length()) < EPS);
    Vec3 intersect = l.o;
    Vec3 n = normal(intersect);
    return getBxDF(intersect)->evaluate(n, -v.d, l.d) * abs(l.d * n);
}

Spectrum Shape::evaluateBxDF(const Ray &v, const Ray &l, double dis) const {
    assert(abs(1 - v.d.length()) < EPS);
    assert(abs(1 - l.d.length()) < EPS);
    Vec3 intersect = l.o;
    Vec3 n = normal(intersect);
    return getBxDF(intersect)->evaluate(n, -v.d, l.d) * abs(l.d * n) / pow(dis, 2)
        * (n * l.d > 0 ? outside : inside)->evaluate(dis);
}

Spectrum Shape::sampleBxDF(const Ray &v, Ray &l, shared_ptr<Medium> &medium, Sampler &RNG) const {
    assert(abs(1 - v.d.length()) < EPS);
    double t;
    bool f = intersect(v, t);
    assert(f);
    Vec3 intersect = v.o + v.d * t;
    l.o = intersect;
    Vec3 n = normal(intersect);
    Spectrum s = getBxDF(intersect)->sample(n, -v.d, l.d, RNG);
    if (n * v.d < 0) assert(medium == outside); else assert(medium == inside);
    medium = (n * l.d > 0 ? outside : inside);
    return s * abs(l.d * n);
}

Spectrum Shape::evaluateLight(const Ray &v) const {
    if (!isLight()) return Spectrum(0);
    assert(abs(1 - v.d.length()) < EPS);
    double t;
    bool f = intersect(v, t);
    assert(f);
    Vec3 intersect = v.o + v.d * t;
    Vec3 n = normal(intersect);
    return getLight(intersect)->evaluate(n, -v.d);
}

Spectrum Shape::sampleLight(const Vec3 &ref, Ray &v, double &t, Sampler &RNG) const {
    if (!isLight()) return Spectrum(0);
    Vec3 pos;
    double pdf;
    sample(pos, pdf, RNG);
    t = (pos - ref).length();
    v = Ray(ref, (pos - ref).norm());
    Vec3 n = normal(pos);
    return getLight(pos)->evaluate(n, -v.d) / pdf * abs(v.d * n);
}

Spectrum Shape::sampleLight(Ray &vB, shared_ptr<Medium> &medium, Sampler &RNG) const {
    if (!isLight()) return Spectrum(0);
    double pdf;
    sample(vB.o, pdf, RNG);
    Vec3 n = normal(vB.o);
    Spectrum s = getLight(vB.o)->sample(n, vB.d, RNG) / pdf;
    medium = (n * vB.d > 0 ? outside : inside);
    return s * abs(vB.d * n);
}

void Shape::setIdentifier(const string &Name) {
    this->name = Name;
}
#endif
#endif /* shape_hpp */
