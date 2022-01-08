#ifndef shapes_revSurface_hpp
#define shapes_revSurface_hpp

#include "../shape.hpp"

class RevSurfaceShape : public Shape {
public:
    RevSurfaceShape(
        const BxDF *bxdf, const Light *light,
        const Medium *inside, const Medium *outside,
        Ray v, const Curve* XCurve, const Curve* YCurve
    );

    bool intersect(const Ray &ray, double &t) const override;
    Vec3 normal(const Vec3 &pos) const override;

private:

    Mat3 T, TInv;
    Ray v; const Curve* XCurve, *YCurve;
};

#ifdef ARC_IMPLEMENTATION

RevSurfaceShape::RevSurfaceShape(
    const BxDF *bxdf, const Light *light,
    const Medium *inside, const Medium *outside,
    Ray v, const Curve* XCurve, const Curve* YCurve
) : Shape(bxdf, light, inside, outside), v(v), XCurve(XCurve), YCurve(YCurve) {
    for (int i = XCurve->k; i <= XCurve->n; ++ i) {
        box = box + discBox(v, {XCurve->B[i](XCurve->L[i]), YCurve->B[i](YCurve->L[i])});
        box = box + discBox(v, {XCurve->B[i](XCurve->U[i]), YCurve->B[i](YCurve->U[i])});
    }
    rotateAxis(v.d, v.d, T, TInv);
}

bool RevSurfaceShape::intersect(const Ray &ray, double &t) const {
    Vec3 Td = T * ray.d;
    Vec3 To = T * (ray.o - v.o);

    t = INF;
    bool ok = false;

    for (int i = curve->getK(); i <= curve->getN(); ++ i) {
        double l, u;
        Poly x1, y1, x2, y1_2, y2_2;
        curve->query(i, l, u, x1, y1);
        y1_2 = y1 * y1;
        x2 = GenPoly(To.z(), Td.z());
        y2_2 = GenPoly(To.x(), Td.x()) * GenPoly(To.x(), Td.x()) + GenPoly(To.y(), Td.y()) * GenPoly(To.y(), Td.y());
        Poly f = y2_2((x1 - GenPoly(x2[0])) / x2[1]) - y1_2;

        auto r = Root(f.stripe());

        for (double t1 : r) {
            if (l < t1 && t1 < u) {
                double t2 = (x1(t1) - x2[0]) / x2[1];
                if (t2 > EPS) t = min(t, t2), ok = true;
#pragma omp critical
                cerr << f(t1) << endl;
//                        cerr << x1(t1) << " " << x2(t2) << " " << y1_2(t1) << " " << y2_2(t2) << " " << t1 << " " << f(t1) << endl;
            }
        }
    }
    return ok;
}
Vec3 RevSurfaceShape::normal(const Vec3 &pos) const {
    Vec3 TPos = T * (pos - v.o);
    for (const auto & face : faces)
        if (face.v0.y() - EPS < TPos.z() && TPos.z() < face.v1.y() + EPS) {
            Vec2 vn = (face.vn0 * (TPos.z() - face.v0.y()) + face.vn1 * (face.v1.y() - TPos.z())).norm();
            Vec3 Tn(
                TPos.x() / TPos.xy().length() * vn.x(),
                TPos.y() / TPos.xy().length() * vn.x(),
                vn.y()
            );
            return TInv * Tn;
        }
    assert(0);
    return Vec3(0);
}

#endif
#endif /* shapes_revSurface_hpp */
