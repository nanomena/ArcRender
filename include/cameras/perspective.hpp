#ifndef cameras_perspective_hpp
#define cameras_perspective_hpp

#include "../camera.hpp"

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(Vec3 o, Vec3 x, Vec3 y, double zLength);
    PerspectiveCamera(Vec3 o, Vec3 x, Vec3 y, double zLength, double f, double R, double f0);

    double evaluate(const Vec3 &pos, Ray &ray, Vec2 &t, Sampler &RNG) const override;
    void sample(Ray &ray, const Vec2 &t, Sampler &RNG) const override;

private:
    double f, R, f0;
    Vec3 o, x, y, z;
    Mat3 T, TInv;
};

#ifdef ARC_IMPLEMENTATION

PerspectiveCamera::PerspectiveCamera(Vec3 o, Vec3 x, Vec3 y, double zLength)
        : o(o), x(x), y(y), f(0), R(0), f0(0) {
    z = (x ^ y).norm(zLength);
    rotateAxis(z.norm(), x.norm(), T, TInv);
}

PerspectiveCamera::PerspectiveCamera(Vec3 o, Vec3 x, Vec3 y, double zLength, double f, double R, double f0)
        : o(o), x(x), y(y), f(f), R(R), f0(f0) {
    z = (x ^ y).norm(zLength);
    rotateAxis(z.norm(), x.norm(), T, TInv);
}

double PerspectiveCamera::evaluate(const Vec3 &pos, Ray &ray, Vec2 &t, Sampler &RNG) const {
    Vec3 vT = T * (pos - o);
    if (-vT.z() < f0) return 0;
    t = Vec2(vT.x() / (-vT.z()) * z.length() / x.length(), vT.y() / (-vT.z()) * z.length() / y.length());
    Vec3 oo = o;

    if (R > 0) {
        double C = (1 - ((1 / f0) - (1 / -vT.z())) / ((1 / f0) - (1 / f))) * (R * ((1 / f0) - (1 / f)));
        double theta = RNG.rand(0, 2 * pi), r2 = RNG.rand(0, 1);
        t += Vec2(cos(theta) * z.length() / x.length(), sin(theta) * z.length() / y.length()) * C * sqrt(r2);
        oo += (x.norm() * cos(theta) + y.norm() * sin(theta)) * sqrt(r2) * R;
    }
    ray = {oo, (pos - oo).norm()};
    return 1;
}

void PerspectiveCamera::sample(Ray &ray, const Vec2 &t, Sampler &RNG) const {
    Vec3 oo = o;
    Vec3 pos = o + (x * t.x() - y * t.y() - z);
    if (R > 0) {
        double theta = RNG.rand(0, 2 * pi), r2 = RNG.rand(0, 1);
        oo += (x.norm() * cos(theta) + y.norm() * sin(theta)) * sqrt(r2) * R;
        pos = o + (x * t.x() - y * t.y() - z) * f / z.length();
    }

    ray = {oo, (pos - oo).norm()};
}

#endif
#endif /* cameras_perspective_hpp */
