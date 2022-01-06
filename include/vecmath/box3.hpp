#ifndef ARC_INCLUDE_VECMATH_BOX3_HPP
#define ARC_INCLUDE_VECMATH_BOX3_HPP

class Box3 {
public:
    Box3();
    explicit Box3(const Vec3 &p);
    Box3(const Vec3 &l, const Vec3 &u);

    Vec3 L() const;
    Vec3 U() const;

    double volume() const;
    bool intersect(const Ray &ray, double &t) const;
    bool intersectCheck(const Vec3 &o, double r) const; // may report true when false

protected:
    Vec3 l, u;
} __attribute__((aligned(64)));

Box3 operator +(const Box3 &b0, const Box3 &b1);
Box3 discBox(Ray v, Vec2 r);

#ifdef ARC_IMPLEMENTATION

Box3::Box3() : l(INF, INF, INF), u(-INF, -INF, -INF) {}
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

bool Box3::intersectCheck(const Vec3 &o, double r) const {
    for (int i = 0; i < 3; ++ i) {
        if ((o[i] + r < l[i]) || (o[i] - r > u[i])) return false;
    }
    return true;
}

double Box3::volume() const {
    double v = 1;
    for (int i = 0; i < 3; ++ i)
        v *= max(0., u[i] - l[i]);
    return v;
}

Box3 operator +(const Box3 &b0, const Box3 &b1) {
    return {
        Vec3(min(b0.L()[0], b1.L()[0]), min(b0.L()[1], b1.L()[1]), min(b0.L()[2], b1.L()[2])),
        Vec3(max(b0.U()[0], b1.U()[0]), max(b0.U()[1], b1.U()[1]), max(b0.U()[2], b1.U()[2])),
    };
}

Box3 discBox(Ray v, Vec2 r) {
    Vec3 h(v.d.yz().squaredLength(), v.d.xz().squaredLength(), v.d.xy().squaredLength());
//    cerr << "H: " << h << endl;
    return {v.o - h * r.x() + v.d * r.y(), v.o + h * r.x() + v.d * r.y()};
}

#endif
#endif //ARC_INCLUDE_VECMATH_BOX3_HPP
