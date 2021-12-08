#ifndef ARC_INCLUDE_VECMATH_BOX3_HPP
#define ARC_INCLUDE_VECMATH_BOX3_HPP

class Box3 {
public:
    Box3();
    explicit Box3(const Vec3 &p);
    Box3(const Vec3 &l, const Vec3 &u);

    Vec3 L() const;
    Vec3 U() const;

    bool intersect(const Ray &ray, double &t) const;

protected:
    Vec3 l, u;
} __attribute__((aligned(64)));

Box3 operator +(const Box3 &b0, const Box3 &b1);

#ifdef ARC_IMPLEMENTATION

Box3::Box3() : l(-INF, -INF, -INF), u(INF, INF, INF) {}
Box3::Box3(const Vec3 &p) : l(p), u(p) {}
Box3::Box3(const Vec3 &l, const Vec3 &u) : l(l), u(u) {}

Vec3 Box3::L() const { return l; }
Vec3 Box3::U() const { return u; }

bool Box3::intersect(const Ray &ray, double &t) const {
    double L = 0, U = 1e100;
#pragma unroll
    for (int i = 0; i < 3; ++i) {
        double Li = (l[i] - ray.o[i]) / ray.d[i], Ui = (u[i] - ray.o[i]) / ray.d[i];
        if (Li > Ui) swap(Li, Ui);
        L = max(L, Li), U = min(U, Ui);
    }
    if (L > U) return false;
    t = L;
    return true;
}

Box3 operator +(const Box3 &b0, const Box3 &b1) {
    return {
        Vec3(min(b0.L()[0], b1.L()[0]), min(b0.L()[1], b1.L()[1]), min(b0.L()[2], b1.L()[2])),
        Vec3(max(b0.U()[0], b1.U()[0]), max(b0.U()[1], b1.U()[1]), max(b0.U()[2], b1.U()[2])),
    };
}

#endif
#endif //ARC_INCLUDE_VECMATH_BOX3_HPP
