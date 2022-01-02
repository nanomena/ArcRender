#ifndef ARCRENDER_INCLUDE_VECMATH_TRANS3_HPP
#define ARCRENDER_INCLUDE_VECMATH_TRANS3_HPP

struct Trans3 {
    Trans3();
    Trans3(const Vec3 &o, const Mat3 &t);
    Trans3(const Vec3 &o, const Vec3 &x, const Vec3 &y, const Vec3 &z);

    Vec3 o; Mat3 t;
};

Vec3 operator *(const Trans3 &t0, const Vec3 &v1);

#ifdef ARC_IMPLEMENTATION

Trans3::Trans3() = default;
Trans3::Trans3(const Vec3 &o, const Mat3 &t) : o(o), t(t) {}
Trans3::Trans3(const Vec3 &o, const Vec3 &x, const Vec3 &y, const Vec3 &z)
    : o(o), t(makeAxisInv(x, y, z)) {}

Vec3 operator *(const Trans3 &t0, const Vec3 &v1) {
    return t0.o + t0.t * v1;
}

#endif
#endif //ARCRENDER_INCLUDE_VECMATH_TRANS3_HPP
