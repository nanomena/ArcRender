#ifndef shapes_conical_hpp
#define shapes_conical_hpp

#include "../shape.hpp"

class Conical : public Shape {
public:
    Conical(
        const BxDF *bxdf, const Light *light,
        const Medium *inside, const Medium *outside,
        Ray v, Vec2 v0, Vec2 v1, Vec2 vn0, Vec2 vn1
    );

    bool intersect(const Ray &ray, double &t) const override;
    Vec3 normal(const Vec3 &pos) const override;

private:

    Mat3 T, TInv;
    Ray v; double hlx0, hlx1, hly, hux0, hux1, huy, vy0, vy1, vx;
    Vec2 vn0, vn1;
};

#ifdef ARC_IMPLEMENTATION

Conical::Conical(
    const BxDF *bxdf, const Light *light,
    const Medium *inside, const Medium *outside,
    Ray v, Vec2 v0, Vec2 v1, Vec2 vn0, Vec2 vn1
) : Shape(bxdf, light, inside, outside), v(v), vn0(vn0), vn1(vn1) {
    double mx = (v0.x() + v1.x()) / 2;
    if (v0.x() < v1.x()) {
        hlx0 = v0.x(), hlx1 = mx, hly = v0.y();
        vy0 = v0.y(), vy1 = v1.y(), vx = mx;
        hux0 = mx, hux1 = v1.x(), huy = v1.y();
    } else {
        hlx0 = mx, hlx1 = v0.x(), hly = v0.y();
        vy0 = v0.y(), vy1 = v1.y(), vx = mx;
        hux0 = v1.x(), hux1 = mx, huy = v1.y();
    }
    double r = max(v0.x() + abs(v0.y()), v1.x() + abs(v1.y()));
    box = discBox(v, v0) + discBox(v, v1);
    rotateAxis(v.d, v.d, T, TInv);
    cerr << "Conical Begin" << endl;
    cerr << v.o << " " << v.d << " " << v0 << " " << v1 << " " << vn0 << " " << vn1 << endl;
    cerr << vy0 << " " << vy1 << " : " << vx << endl;
    cerr << box.L() << " " << box.U() << endl;
    cerr << "Conical End" << endl;
}

bool Conical::intersect(const Ray &ray, double &t) const {
    if (abs((T * ray.d).z()) < EPS) return false;

    Ray TRay = Ray(T * (ray.o - v.o), T * ray.d);

    double tl1 = (hly - TRay.o.z()) / TRay.d.z();
    bool okl1 = tl1 > -EPS && hlx0 <= (TRay.o + TRay.d * tl1).xy().length() && (TRay.o + TRay.d * tl1).xy().length() <= hlx1;
    double tu1 = (huy - TRay.o.z()) / TRay.d.z();
    bool oku1 = tu1 > -EPS && hux0 <= (TRay.o + TRay.d * tu1).xy().length() && (TRay.o + TRay.d * tu1).xy().length() <= hux1;

    bool ok2 = false;
    double t2 = 0;
    if (abs(TRay.d.z()) >= EPS) {
        double x = -TRay.o.xy() * TRay.d.xy().norm(), y = (TRay.o.xy() + TRay.d.xy().norm() * x).length();
        if (vx >= y) {
            {
                t2 = (x - sqrt(vx * vx - y * y)) / TRay.d.xy().length();
                ok2 = t2 > -EPS && vy0 <= (TRay.o + TRay.d * t2).z() && (TRay.o + TRay.d * t2).z() <= vy1;
            }
            if (!ok2) {
                t2 = (x + sqrt(vx * vx - y * y)) / TRay.d.xy().length();
                ok2 = t2 > -EPS && vy0 <= (TRay.o + TRay.d * t2).z() && (TRay.o + TRay.d * t2).z() <= vy1;
            }
        }
    }

    t = INF;
    bool ok = false;
    if (okl1) ok = true, t = min(t, tl1);
    if (oku1) ok = true, t = min(t, tu1);
    if (ok2) ok = true, t = min(t, t2);
    if (t < EPS) ok = false;
    return ok;
}

Vec3 Conical::normal(const Vec3 &pos) const {
    Vec3 TPos = T * (pos - v.o);
    Vec2 vn = (vn0 * (TPos.z() - vy0) + vn1 * (vy1 - TPos.z())).norm();
    Vec3 Tn(
        TPos.x() / TPos.xy().length() * vn.x(),
        TPos.y() / TPos.xy().length() * vn.x(),
        vn.y()
    );
    assert(abs(Tn.length() - 1) < EPS);
    return TInv * Tn;
}

#endif
#endif /* shapes_conical_hpp */
