#ifndef shape_hpp
#define shape_hpp

#include "utils.hpp"
#include "vecmath.hpp"
#include "bxdf.hpp"
#include "light.hpp"
#include "medium.hpp"
#include "object.hpp"
#include "texture.hpp"

class Shape : public Object {
public:
    Shape(
        const BxDF *bxdf, const Light *light,
        const Medium *inside, const Medium *outside
    );

    bool isLight() const override;

    virtual bool intersect(const Ray &ray, double &t, Vec3 &pos, Vec2 &texPos) const {
        throw invalid_argument("NotImplementedError");
    }
    virtual Vec3 normal(const Vec2 &texPos) const {
        throw invalid_argument("NotImplementedError");
    } // norm should be unitary
    void sampleSurface(Vec3 &pos, Vec2 &texPos, double &pdf, Sampler &RNG) const override {
        throw invalid_argument("NotImplementedError");
    }
    double evaluateSurfaceImportance(const Vec3 &pos, const Vec2 &texPos) const override {
        throw invalid_argument("NotImplementedError");
    }
    const Medium *getMedium(const Vec2 &texPos, const Vec3 &l) const;

    Spectrum evaluateBxDF(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, const Vec3 &l) const override;
    Spectrum sampleBxDF(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, Vec3 &l, const Medium *&medium, Sampler &RNG) const override;
    double evaluateBxDFImportance(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, const Vec3 &l) const override;

    Spectrum evaluateLight(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos) const override;
    Spectrum evaluateLightBack(const Vec3 &lB, const Vec3 &pos, const Vec2 &texPos) const override;
    Spectrum sampleLight(Vec3 &lB, Vec3 &pos, Vec2 &texPos, const Medium *&medium, Sampler &RNG) const override;
    double evaluateLightImportance(const Vec3 &lB, const Vec3 &pos, const Vec2 &texPos) const override;

    virtual Box3 outline() const;

protected:

    const BxDF *bxdf;
    const Light *light;
    const Medium *inside, *outside;
    Box3 box;
};

#ifdef ARC_IMPLEMENTATION

Shape::Shape(
    const BxDF *bxdf, const Light *light,
    const Medium *inside, const Medium *outside
) : bxdf(bxdf), light(light), inside(inside), outside(outside) {}

bool Shape::isLight() const {
    return light != nullptr;
}
Box3 Shape::outline() const {
    return box;
}

const Medium *Shape::getMedium(const Vec2 &texPos, const Vec3 &l) const {
    Vec3 n = normal(texPos);
    return (n * l > 0 ? outside : inside);
}

Spectrum Shape::evaluateBxDF(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, const Vec3 &l) const {
    assert(abs(1 - v.length()) < EPS);
    assert(abs(1 - l.length()) < EPS);
    Vec3 n = normal(texPos);
    return bxdf->evaluate(texPos, n, -v, l) * abs(l * n);
}

Spectrum Shape::sampleBxDF(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, Vec3 &l, const Medium *&medium, Sampler &RNG) const {
    assert(abs(1 - v.length()) < EPS);
    Vec3 n = normal(texPos);
    Spectrum s = bxdf->sample(texPos, n, -v, l, RNG);
    medium = (n * l > 0 ? outside : inside);
    return s * abs(l * n);
}

double Shape::evaluateBxDFImportance(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos, const Vec3 &l) const {
    Vec3 n = normal(texPos);
    return bxdf->evaluateImportance(texPos, n, -v, l) * abs(l * n);
}

Spectrum Shape::evaluateLight(const Vec3 &v, const Vec3 &pos, const Vec2 &texPos) const {
    if (!isLight()) return Spectrum(0);
    assert(abs(1 - v.length()) < EPS);
    Vec3 n = normal(texPos);
    return light->evaluate(texPos, n, -v);
}

Spectrum Shape::evaluateLightBack(const Vec3 &lB, const Vec3 &pos, const Vec2 &texPos) const {
    Vec3 n = normal(texPos);
    return light->evaluate(texPos, n, lB) * abs(lB * n);
}

Spectrum Shape::sampleLight(Vec3 &lB, Vec3 &pos, Vec2 &texPos, const Medium *&medium, Sampler &RNG) const {
    if (!isLight()) return Spectrum(0);
    double pdf;
    sampleSurface(pos, texPos, pdf, RNG);
    Vec3 n = normal(texPos);
    Spectrum s = light->sample(texPos, n, lB, RNG) / pdf;
    medium = (n * lB > 0 ? outside : inside);
    return s * abs(lB * n);
}

double Shape::evaluateLightImportance(const Vec3 &lB, const Vec3 &pos, const Vec2 &texPos) const {
    if (!isLight()) return 0;
    Vec3 n = normal(texPos);
    return light->evaluateImportance(texPos, n, lB);
}

#endif
#endif /* shape_hpp */