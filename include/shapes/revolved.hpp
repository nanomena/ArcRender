#ifndef shapes_revolved_hpp
#define shapes_revolved_hpp

#include "../shape.hpp"

class Revolved : public Shape {
public:
    Revolved(
        const BxDF *bxdf, const Light *light,
        const Medium *inside, const Medium *outside,
        Ray v, const Spline* Curve, const TextureMap &normalMap = TextureMap(Spectrum(0, 0, 1))
    );

    bool intersect(const Ray &ray, double &t, Vec3 &pos, Vec2 &texPos) const override;
    Vec3 normal(const Vec2 &texPos) const override;

private:

    TextureMap normalMap;
    Mat3 T, TInv;
    Ray v; const Spline *Curve;
};

#ifdef ARC_IMPLEMENTATION

Revolved::Revolved(
    const BxDF *bxdf, const Light *light,
    const Medium *inside, const Medium *outside,
    Ray v, const Spline* Curve, const TextureMap &normalMap
) : Shape(bxdf, light, inside, outside), v(v), Curve(Curve), normalMap(normalMap) {
    for (int i = Curve->k; i <= Curve->n + 1; ++ i)
        box = box + discBox(v, {Curve->B(Curve->tdx(i)).x(), Curve->B(Curve->tdx(i)).y()});
    rotateAxis(v.d, v.d, T, TInv);
}

bool Revolved::intersect(const Ray &ray, double &t, Vec3 &pos, Vec2 &texPos) const {
    Ray TRay = Ray(T * (ray.o - v.o), T * ray.d);


//    int okx = 0;
//    double vx = 0.5, vy0 = 0, vy1 = 1;
//    double x = -TRay.o.xy() * TRay.d.xy().norm(), y = (TRay.o.xy() + TRay.d.xy().norm() * x).length();
//    Vec2 texG;
//    if (vx >= y) {
//        {
//            t = (x - sqrt(vx * vx - y * y)) / TRay.d.xy().length();
//            okx = t > EPS && vy0 <= (TRay.o + TRay.d * t).z() && (TRay.o + TRay.d * t).z() <= vy1;
//        }
//        if (!okx) {
//            t = (x + sqrt(vx * vx - y * y)) / TRay.d.xy().length();
//            okx = t > EPS && vy0 <= (TRay.o + TRay.d * t).z() && (TRay.o + TRay.d * t).z() <= vy1;
//        }
//        if (okx) {
//            pos = ray.o + ray.d * t;
//            Vec3 TPos = TRay.o + TRay.d * t;
//            texG = {atan2(TPos.y(), TPos.x()), (TPos.z() - vy0) / (vy1 - vy0)};
//        }
//    }
//    texPos = texG;
//    return okx;


    bool ok = false;
    t = INF;
    for (int i = Curve->k; i <= Curve->n; ++ i)
        for (int j = 0; j <= Curve->k; ++ j) {
            double u = (Curve->tdx(i) * (Curve->k - j) + Curve->tdx(i + 1) * j) / (Curve->k);

            Vec2 B = Curve->B(u);
            double tCur = (B.y() - TRay.o.z()) / TRay.d.z();
            double y = (TRay.o + TRay.d * tCur).xy().length() - B.x();

            int iter = 0, flag;
            do {
                flag = false;
                Vec2 dB = Curve->dB(u);
                double dy = (TRay.o.xy() + TRay.d.xy() * tCur).norm() * (TRay.d.xy() / TRay.d.z()) * dB.y() - dB.x();
//                if (okx) {
//                    cerr << "Iter Info Begin" << endl;
//                    cerr << TRay.o << " " << TRay.d << endl;
//                    cerr << u << endl;
//                    cerr << dB.y() << endl;
//                    cerr << (TRay.o.xy() + TRay.d.xy() * tCur).length() << " " << B.x() << endl;
//                    cerr << (TRay.o.xy() + TRay.d.xy() * tCur).norm() * (TRay.d.xy() / TRay.d.z()) * dB.y() << " " << dB.x() << endl;
//                    cerr << "Iter Info End" << endl;
//                }
                double step = 1;
                for (double step = 1; step > 0.04; step *= 0.75) {
                    double u_ = u - y / dy * step;
                    if (u_ < Curve->tdx(Curve->k) || u_ > Curve->tdx(Curve->n + 1)) continue;
                    B = Curve->B(u_);
                    tCur = (B.y() - TRay.o.z()) / TRay.d.z();
                    double y_ = (TRay.o + TRay.d * tCur).xy().length() - B.x();
                    if (abs(y_) < abs(y)) {
                        u = u_;
                        y = y_;
                        flag = true;
                        break;
                    }
                }
            } while (abs(y) > EPS && ++iter < 15 && flag);
            if (abs(y) < EPS && tCur < t && tCur > EPS) {
                ok = true;
                t = tCur;
                pos = ray.o + ray.d * t;
                texPos = {
                    fmod(atan2((TRay.o + TRay.d * tCur).y(), (TRay.o + TRay.d * tCur).x()) / (2 * pi) + 1, 1),
                    u
                };
            }
        }
//#pragma omp critical
//    if (ok) {
//        cerr << texG << " " << texPos << endl;
//    }


    return ok;
}
Vec3 Revolved::normal(const Vec2 &texPos) const {
    Vec2 dB = Curve->dB(texPos.y()).norm();
    Vec3 Tz = {cos(texPos.x() * (2 * pi)) * dB.y(), sin(texPos.x() * (2 * pi)) * dB.y(), -dB.x()};
    Vec3 Tx = {sin(texPos.x() * (2 * pi)), -cos(texPos.x() * (2 * pi)), 0};
    Vec3 Ty = (Tz ^ Tx).norm();
    Spectrum LNormal = normalMap[texPos];
    Vec3 TNormal = (LNormal.r * Tx + LNormal.g * Ty + LNormal.b * Tz).norm();
    return TInv * Tz;
}

#endif
#endif /* shapes_revolved_hpp */
