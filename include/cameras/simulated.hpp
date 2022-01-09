#ifndef cameras_simulated_hpp
#define cameras_simulated_hpp

#include "../camera.hpp"

class SimulatedCamera : public Camera {
public:
    SimulatedCamera(Vec3 o, Vec3 x, Vec3 y, double zLength, double f, double R, double f0);

    double evaluate(const Vec3 &pos, Ray &ray, Vec2 &t, Sampler &RNG) const override;
    void sample(Ray &ray, const Vec2 &t, Sampler &RNG) const override;

private:
    double f, R, f0;
    Vec3 o, x, y, z;
    Mat3 T, TInv;
};

#ifdef ARC_IMPLEMENTATION

SimulatedCamera::SimulatedCamera(Vec3 o, Vec3 x, Vec3 y, double zLength, double f, double R, double f0)
        : o(o), x(x), y(y), f(f), R(R), f0(f0) {
    z = (x ^ y).norm(zLength);
    rotateAxis(z.norm(), x.norm(), T, TInv);
}

double SimulatedCamera::evaluate(const Vec3 &pos, Ray &ray, Vec2 &t, Sampler &RNG) const {
    Vec3 vT = T * (pos - o);
    if (-vT.z() < f0) return 0;

    double C = (1 - ((1 / f0) - (1 / -vT.z())) / ((1 / f0) - (1 / f))) * (R * ((1 / f0) - (1 / f)));
    double theta = RNG.rand(0, 2 * pi), r2 = RNG.rand(0, 1);

    t = Vec2(vT.x() / (-vT.z()) * z.length() / x.length(), vT.y() / (-vT.z()) * z.length() / y.length())
        + Vec2(cos(theta) * z.length() / x.length(), sin(theta) * z.length() / y.length()) * C * sqrt(r2);

    Vec3 oo = o + (x.norm() * cos(theta) + y.norm() * sin(theta)) * sqrt(r2) * R;
    ray = {oo, (pos - oo).norm()};
    return 1;
}

void SimulatedCamera::sample(Ray &ray, const Vec2 &t, Sampler &RNG) const {
    double theta = RNG.rand(0, 2 * pi), r2 = RNG.rand(0, 1);

    Vec3 oo = o + (x.norm() * cos(theta) + y.norm() * sin(theta)) * sqrt(r2) * R;
    ray = {oo, (o + (x * t.x() - y * t.y() - z) * f / z.length() - oo).norm()};
}

#endif
#endif /* cameras_simulated_hpp */
