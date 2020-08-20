#ifndef shape_hpp
#define shape_hpp

#include "utils.hpp"
#include "geometry.hpp"

class Shape
{

protected:
    Cuboid box;

public:
    virtual void inter(const Ray &ray, int &is_inter, Vec3 &intersect) const;
    virtual Vec3 normal(const Vec3 &inter) const; // norm should be unitary
    virtual void sample(Vec3 &pos, double &pdf) const;

    virtual Cuboid outline() const;
};

#ifdef ARC_IMPLEMENTATION

void Shape::inter(const Ray &ray, int &is_inter, Vec3 &intersect) const
{
    throw invalid_argument("NotImplementedError");
}
Vec3 Shape::normal(const Vec3 &inter) const
{
    throw invalid_argument("NotImplementedError");
}
void Shape::sample(Vec3 &pos, double &pdf) const
{
    throw invalid_argument("NotImplementedError");
}
Cuboid Shape::outline() const
{
    return box;
}

#endif
#endif /* shape_hpp */
