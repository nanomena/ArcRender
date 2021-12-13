#ifndef shape_hpp
#define shape_hpp

#include "utils.hpp"
#include "vecmath.hpp"
#include "bxdf.hpp"
#include "light.hpp"

class Shape {
public:
    Shape(const shared_ptr<BxDF> &bxdf, const shared_ptr<Light> &light);

    virtual bool isLight() const;

    virtual bool intersect(const Ray &ray, double &t) const;
    virtual Vec3 normal(const Vec3 &inter) const; // norm should be unitary

    Spectrum evaluateVtL(const Ray &v, const Ray &l) const;
    Spectrum evaluateLtV(const Ray &lB, const Ray &vB) const;
    Spectrum sampleVtL(const Ray &v, Ray &l) const;
    Spectrum sampleLtV(const Ray &lB, Ray &vB) const;

    Spectrum evaluateVtS(const Ray &v) const;
    Spectrum evaluateStV(const Ray &vB) const;
    Spectrum sampleS(const Vec3 &ref, Ray &v, double &t) const;
    Spectrum sampleStV(Ray &vB) const;
    virtual Box3 outline() const;

    void setIdentifier(const string &Name);
    string name;

protected:
    virtual shared_ptr<BxDF> getBxDF(const Vec3 &pos) const;
    virtual shared_ptr<Light> getLight(const Vec3 &pos) const;
    virtual void sample(Vec3 &pos, double &pdf) const;

    shared_ptr<BxDF> bxdf;
    shared_ptr<Light> light;
    Box3 box;
};

#ifdef ARC_IMPLEMENTATION

Shape::Shape(const shared_ptr<BxDF> &bxdf, const shared_ptr<Light> &light) : bxdf(bxdf), light(light) {}
bool Shape::intersect(const Ray &ray, double &t) const {
    throw invalid_argument("NotImplementedError");
}
Vec3 Shape::normal(const Vec3 &inter) const {
    throw invalid_argument("NotImplementedError");
}
void Shape::sample(Vec3 &pos, double &pdf) const {
    throw invalid_argument("NotImplementedError");
}
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

Spectrum Shape::evaluateVtL(const Ray &v, const Ray &l) const {
    assert(abs(1 - v.d.length()) < EPS);
    assert(abs(1 - l.d.length()) < EPS);
    Vec3 intersect = l.o;
    Vec3 n = normal(intersect);
    return getBxDF(intersect)->evaluateVtL(n, -v.d, l.d) * abs(l.d * n);
}

Spectrum Shape::evaluateLtV(const Ray &lB, const Ray &vB) const {
    assert(abs(1 - lB.d.length()) < EPS);
    assert(abs(1 - vB.d.length()) < EPS);
    Vec3 intersect = vB.o;
    Vec3 n = normal(intersect);
    return getBxDF(intersect)->evaluateVtL(n, vB.d, -lB.d) * abs(vB.d * n);
}

Spectrum Shape::sampleVtL(const Ray &v, Ray &l) const {
    assert(abs(1 - v.d.length()) < EPS);
    double t;
    bool f = intersect(v, t);
    assert(f);
    Vec3 intersect = v.o + v.d * t;
    l.o = intersect;
    Vec3 n = normal(intersect);
    Spectrum s = getBxDF(intersect)->sampleVtL(n, -v.d, l.d);
    return s * abs(l.d * n);
}

Spectrum Shape::sampleLtV(const Ray &lB, Ray &vB) const {
    assert(abs(1 - lB.d.length()) < EPS);
    double t;
    bool f = intersect(lB, t);
    assert(f);
    Vec3 intersect = lB.o + lB.d * t;
    vB.o = intersect;
    Vec3 n = normal(intersect);
    Spectrum s = getBxDF(intersect)->sampleLtV(n, -lB.d, vB.d);
    return s * abs(vB.d * n);
}

Spectrum Shape::evaluateVtS(const Ray &v) const {
    if (!isLight()) return Spectrum(0);
    assert(abs(1 - v.d.length()) < EPS);
    double t;
    bool f = intersect(v, t);
    assert(f);
    Vec3 intersect = v.o + v.d * t;
    Vec3 n = normal(intersect);
    return getLight(intersect)->evaluate(n, -v.d);
}

Spectrum Shape::evaluateStV(const Ray &vB) const {
    if (!isLight()) return Spectrum(0);
    assert(abs(1 - vB.d.length()) < EPS);
    Vec3 intersect = vB.o;
    Vec3 n = normal(intersect);
    return getLight(intersect)->evaluate(n, vB.d) * abs(vB.d * n);
}

Spectrum Shape::sampleS(const Vec3 &ref, Ray &v, double &t) const {
    if (!isLight()) return Spectrum(0);
    Vec3 pos;
    double pdf;
    sample(pos, pdf);
    t = (pos - ref).length();
    v = Ray(ref, (pos - ref).norm());
    Vec3 n = normal(pos);
    return getLight(pos)->evaluate(n, -v.d) / pdf * abs(v.d * n);
}

Spectrum Shape::sampleStV(Ray &vB) const {
    if (!isLight()) return Spectrum(0);
    double pdf;
    sample(vB.o, pdf);
    Vec3 n = normal(vB.o);
    Spectrum s = getLight(vB.o)->sampleStV(n, vB.d) / pdf;
    return s * abs(vB.d * n);
}

void Shape::setIdentifier(const string &Name) {
    this->name = Name;
}
#endif
#endif /* shape_hpp */
