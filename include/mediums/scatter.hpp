#ifndef mediums_scatter_hpp
#define mediums_scatter_hpp

#include "../medium.hpp"

class Scatter : public Medium {
public:
    explicit Scatter(double lambda, Spectrum color, const Shape *range);
    ~Scatter();

    Spectrum evaluate(const Ray &v, double t) const override;
    Spectrum sample(
        const Scene *scene, const Ray &v, const Object *&object, Vec3 &pos, Vec2 &texPos, Sampler &RNG
    ) const override;

private:
    double lambda;
    const Particle *particle;
    const Shape *range;
};

#ifdef ARC_IMPLEMENTATION

Scatter::Scatter(double lambda, Spectrum color, const Shape *range)
    : lambda(lambda), particle(new Particle(color)), range(range) {}
Scatter::~Scatter() {delete particle;}

Spectrum Scatter::evaluate(const Ray &v, double t) const {
    double td = 0, t0 = 0, t1 = 0;
    while (true) {
        swap(t0, t1);
        double tx;
        Vec3 _; Vec2 _t;
        bool ok = range->intersect({v.o + v.d * td, v.d}, tx, _, _t);
        if (!ok) break;
        t0 += min(td + tx, t) - min(td, t);
        td += tx;
    }
    return Spectrum(1) * exp(-lambda * t1);
}

Spectrum Scatter::sample(
    const Scene *scene, const Ray &v, const Object *&object, Vec3 &pos, Vec2 &texPos, Sampler &RNG
) const {
    double td = 0; bool inside = true;
    while (true) {
        inside = !inside;
        double tx;
        Vec3 _; Vec2 _t;
        bool ok = range->intersect({v.o + v.d * td, v.d}, tx, _, _t);
        if (!ok) break;
        td += tx;
    }

    double t;
    const Shape *shape;
    scene->intersect(v, shape, t, pos, texPos);
    double PDist = log(1 - RNG.rand()) / (-lambda);

    double tGap = 0;
    td = 0;
    while (true) {
        double tx;
        Vec3 _; Vec2 _t;
        bool ok = range->intersect({v.o + v.d * td, v.d}, tx, _, _t);
        if (!ok) tx = INF;
        if (!inside) {
            if (td + tx > t) {
                object = shape;
                break;
            }
            td += tx;
            tGap += tx;
        } else {
            if (tGap + PDist > t) {
                if (td + tx > t) {
                    object = shape;
                    break;
                }
            } else {
                if (td + tx > tGap + PDist) {
                    object = particle;
                    pos = v.o + v.d * (tGap + PDist);
                    texPos = Vec2(0, 0);
                    break;
                }
            }
            td += tx;
        }
        inside = !inside;
    }
    return Spectrum(1);
}



#endif
#endif //mediums_scatter_hpp
