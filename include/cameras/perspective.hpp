#ifndef cameras_perspective_hpp
#define cameras_perspective_hpp

#include "../camera.hpp"

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(Vec3 o, Vec3 x, Vec3 y, double zLength);

    Ray evaluate(const Vec3 &pos, Vec2 &t) const override;
    Ray sample(const Vec2 &t, double &pdf, Sampler &RNG) const override;
    double evaluateImportance(const Vec3 &pos) const override;

private:
    Vec3 o, x, y, z;
};

#ifdef ARC_IMPLEMENTATION

PerspectiveCamera::PerspectiveCamera(Vec3 o, Vec3 x, Vec3 y, double zLength) : o(o), x(x), y(y) {
    z = (x ^ y).norm(zLength);
}

Ray PerspectiveCamera::evaluate(const Vec3 &pos, Vec2 &t) const {
    Mat3 T, TInv;
    rotateAxis(z.norm(), x.norm(), T, TInv);
    Vec3 vT = T * (pos - o);
    t = {-(vT.x() / x.length()) / (vT.z() / z.length()),-(vT.y() / y.length()) / (vT.z() / z.length())};
    return {o, (pos - o).norm()};
}
Ray PerspectiveCamera::sample(const Vec2 &t, double &pdf, Sampler &RNG) const {
    pdf = 1.;
    return {o, (x * t.x() - y * t.y() - z).norm()};
}
double PerspectiveCamera::evaluateImportance(const Vec3 &pos) const {
    Vec2 t;
    if (evaluate(pos, t).d * -z < 0) return INF; else return 1.;
}

#endif
#endif /* cameras_perspective_hpp */
