#ifndef ARC_INCLUDE_VECMATH_RAY_HPP
#define ARC_INCLUDE_VECMATH_RAY_HPP

struct Ray {
    Ray();
    Ray(const Vec3 &o, const Vec3 &d);

    Vec3 o, d;
};

#ifdef ARC_IMPLEMENTATION

Ray::Ray() = default;
Ray::Ray(const Vec3 &o, const Vec3 &d) : o(o), d(d) {}

#endif
#endif //ARC_INCLUDE_VECMATH_RAY_HPP
