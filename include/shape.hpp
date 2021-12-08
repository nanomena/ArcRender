#ifndef shape_hpp
#define shape_hpp

#include "utils.hpp"
#include "vecmath.hpp"

class Shape {

protected:
    Box3 box;

public:
    virtual bool intersect(const Ray &ray, double &t) const;
    virtual Vec3 normal(const Vec3 &inter) const; // norm should be unitary
    virtual void sample(Vec3 &pos, double &pdf) const;

    virtual Box3 outline() const;
};

#ifdef ARC_IMPLEMENTATION

bool Shape::intersect(const Ray &ray, double &t) const {
    throw invalid_argument("NotImplementedError");
}
Vec3 Shape::normal(const Vec3 &inter) const {
    throw invalid_argument("NotImplementedError");
}
void Shape::sample(Vec3 &pos, double &pdf) const {
    throw invalid_argument("NotImplementedError");
}
Box3 Shape::outline() const {
    return box;
}

#endif
#endif /* shape_hpp */
