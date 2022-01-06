#ifndef shapes_revSurface_hpp
#define shapes_revSurface_hpp

#include "../shape.hpp"

class RevSurface : public Shape {
public:
    RevSurface(
        const BxDF *bxdf, const Light *light,
        const Medium *inside, const Medium *outside,
        Ray v, const Curve* curve
    );

    bool intersect(const Ray &ray, double &t) const override;
    Vec3 normal(const Vec3 &pos) const override;

private:

    Mat3 T, TInv;
    Ray v; const Curve* curve;
};

#ifdef ARC_IMPLEMENTATION

RevSurface::RevSurface(
    const BxDF *bxdf, const Light *light,
    const Medium *inside, const Medium *outside,
    Ray v, const Curve* curve
) : Shape(bxdf, light, inside, outside), v(v), curve(curve) {
    box = Box3(v.o - Vec3(r, r, r), v.o + Vec3(r, r, r));
    rotateAxis(v.d, v.d, T, TInv);
}

bool RevSurface::intersect(const Ray &ray, double &t) const {
    Vec3 Td = T * ray.d;
    Vec3 To = T * (ray.o - v.o);

    for (int i = curve->getK(); i <= curve->getN(); ++ i) {
        double l, u;
        Poly x1, y1, x2, y1_2, y2_2;
        curve->query(i, l, u, x1, y1);
        y1_2 = y1 * y1;
        x2 = GenPoly(To.z(), Td.z());
        y2_2 = GenPoly(To.x(), Td.x()) * GenPoly(To.x(), Td.x()) + GenPoly(To.y(), Td.y()) * GenPoly(To.y(), Td.y());
        Poly f = y2_2((x1 - GenPoly(x2[0])) / x2[1]) - y1_2;
    }

}
Vec3 RevSurface::normal(const Vec3 &pos) const {

}

#endif
#endif /* shapes_revSurface_hpp */
