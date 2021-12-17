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
    virtual void sample(Vec3 &pos, double &pdf, Sampler &RNG) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual double evaluatePdf(const Vec3 &pos) const {
        throw invalid_argument("NotImplementedError");
    }
    shared_ptr<Medium> getMedium(const Ray &test) const;
    shared_ptr<Medium> otherSide(const shared_ptr<Medium> &test) const;

    Spectrum evaluateBxDF(const Ray &v, const Ray &l) const;
    Spectrum sampleBxDF(const Ray &v, Ray &l, shared_ptr<Medium> &medium, Sampler &RNG) const;
    double evaluateBxDFPdf(const Ray &v, const Ray &l) const;

    Spectrum evaluateLight(const Ray &v) const;
    Spectrum evaluateLightBack(const Ray &vB) const;
    Spectrum sampleLight(Ray &lB, shared_ptr<Medium> &medium, Sampler &RNG) const;
    double evaluateLightPdf(const Ray &lB) const;

    bool glossy(const Vec3 &pos) const;

    virtual Box3 outline() const;

    void setIdentifier(const string &Name);
    string name;

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

bool Shape::glossy(const Vec3 &pos) const {
    return getBxDF(pos)->glossy();
}

shared_ptr<Medium> Shape::getMedium(const Ray &test) const {
    Vec3 n = normal(test.o);
    return (n * test.d > 0 ? outside : inside);
}

shared_ptr<Medium> Shape::otherSide(const shared_ptr<Medium> &test) const {
    if (test == inside) {
        if (test != outside) return outside;
        else return nullptr;
    } else {
        assert(test == outside);
        return inside;
    }
}


Spectrum Shape::evaluateBxDF(const Ray &v, const Ray &l) const {
    assert(abs(1 - v.d.length()) < EPS);
    assert(abs(1 - l.d.length()) < EPS);
    Vec3 n = normal(l.o);
    return getBxDF(l.o)->evaluate(n, -v.d, l.d) * abs(l.d * n);
}

double Shape::evaluateBxDFPdf(const Ray &v, const Ray &l) const {
    Vec3 n = normal(l.o);
    return getBxDF(l.o)->evaluatePdf(n, -v.d, l.d);
}

Spectrum Shape::sampleBxDF(const Ray &v, Ray &l, shared_ptr<Medium> &medium, Sampler &RNG) const {
    assert(abs(1 - v.d.length()) < EPS);
    double t;
    bool f = intersect(v, t);
    assert(f);
    l.o = v.o + v.d * t;
    Vec3 n = normal(l.o);
    Spectrum s = getBxDF(l.o)->sample(n, -v.d, l.d, RNG);
//    if (n * v.d < 0) assert(medium == outside); else assert(medium == inside);
    medium = getMedium(l);
    return s * abs(l.d * n);
}

Spectrum Shape::evaluateLight(const Ray &v) const {
    if (!isLight()) return Spectrum(0);
    assert(abs(1 - v.d.length()) < EPS);
    double t;
    bool f = intersect(v, t);
    assert(f);
    Vec3 pos = v.o + v.d * t;
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
    sample(lB.o, pdf, RNG);
    Vec3 n = normal(lB.o);
    Spectrum s = getLight(lB.o)->sample(n, lB.d, RNG) / pdf;
    medium = getMedium(lB);
    return s * abs(lB.d * n);
}

double Shape::evaluateLightPdf(const Ray &lB) const {
    if (!isLight()) return 0;
    Vec3 n = normal(lB.o);
    return getLight(lB.o)->evaluatePdf(n, lB.d);
}

void Shape::setIdentifier(const string &Name) {
    this->name = Name;
}
#endif
#endif /* shape_hpp */