#ifndef ARCRENDER_INCLUDE_VECMATH_TRANS3_HPP
#define ARCRENDER_INCLUDE_VECMATH_TRANS3_HPP

struct Trans3 {
    Trans3();
    Trans3(const Vec3 &o, const Mat3 &t);
    Trans3(const Vec3 &o, const Vec3 &x, const Vec3 &y, const Vec3 &z);
    Trans3(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, const Vec3 &vt0, const Vec3 &vt1, const Vec3 &vt2);

    Vec3 o; Mat3 t;
};

Vec3 operator *(const Trans3 &t0, const Vec3 &v1);

Trans3 TransEye();

#ifdef ARC_IMPLEMENTATION

Trans3::Trans3() = default;
Trans3::Trans3(const Vec3 &o, const Mat3 &t) : o(o), t(t) {}
Trans3::Trans3(const Vec3 &o, const Vec3 &x, const Vec3 &y, const Vec3 &z)
    : o(o), t(makeAxisInv(x, y, z)) {}

Trans3::Trans3(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, const Vec3 &vt0, const Vec3 &vt1, const Vec3 &vt2) {
    t = makeAxisInv(vt1 - vt0, vt2 - vt0, (vt1 - vt0) ^ (vt2 - vt0)) *
        makeAxis(v1 - v0, v2 - v0, (v1 - v0) ^ (v2 - v0));
    o = vt0 - t * v0;
}

Vec3 operator *(const Trans3 &t0, const Vec3 &v1) {
    return t0.o + t0.t * v1;
}

Trans3 TransEye() {
    return {Vec3(), Vec3(1, 0, 0), Vec3(0, 1, 0) , Vec3(0, 0, 1)};
}


#endif
#endif //ARCRENDER_INCLUDE_VECMATH_TRANS3_HPP
