#ifndef cameras_perspective_hpp
#define cameras_perspective_hpp

#include "../camera.hpp"

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(Vec3 o, Vec3 x, Vec3 y, double zLength);

    double evaluate(const Vec3 &pos, Ray &ray, Vec2 &t, Sampler &RNG) const override;
    void sample(Ray &ray, const Vec2 &t, Sampler &RNG) const override;

private:
    Vec3 o, x, y, z;
};

#ifdef ARC_IMPLEMENTATION

PerspectiveCamera::PerspectiveCamera(Vec3 o, Vec3 x, Vec3 y, double zLength) : o(o), x(x), y(y) {
    z = (x ^ y).norm(zLength);
}

double PerspectiveCamera::evaluate(const Vec3 &pos, Ray &ray, Vec2 &t, Sampler &RNG) const {
    Mat3 T, TInv;
    rotateAxis(z.norm(), x.norm(), T, TInv);
    Vec3 vT = T * (pos - o);
    if (vT.z() < EPS) return 0;

    t = {-(vT.x() / x.length()) / (vT.z() / z.length()),-(vT.y() / y.length()) / (vT.z() / z.length())};
    ray = {o, (pos - o).norm()};
    return 1;
}
void PerspectiveCamera::sample(Ray &ray, const Vec2 &t, Sampler &RNG) const {
    ray = {o, (x * t.x() - y * t.y() - z).norm()};
}

#endif
#endif /* cameras_perspective_hpp */
