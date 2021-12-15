#ifndef sampler_hpp
#define sampler_hpp

#include "utils.hpp"
#include "vecmath.hpp"

template<int base>
class HaltonSequence {
    long long key;
    double now;

public:
    HaltonSequence() {
        key = 0;
        now = 0;
    }
    double operator ()() {
        double q = 1. / base;
        key += 1;
        for (long long i = key; i % base == 0; now -= (base - 1) * q, i /= base, q /= base);
        now += q;
        return now;
    }
};

class Sampler {
public:
    explicit Sampler();

    double sample();
    double rand(double l = 0, double r = 1);

    Vec2 pixel(Vec2 t, double width, double height);
    Vec3 hemisphere();
    Vec3 sphere();

private:
    unsigned short seeds[3]{};
};

static mt19937 GlobalRandom;

#ifdef ARC_IMPLEMENTATION

Sampler::Sampler() {
    seeds[0] = GlobalRandom();
    seeds[1] = GlobalRandom();
    seeds[2] = GlobalRandom();
}

double Sampler::sample() {
    return erand48(seeds);
}
double Sampler::rand(double l, double r) {
    return sample() * (r - l) + l;
}

Vec2 Sampler::pixel(Vec2 t, double width, double height) {
    double _x = sample(), _y = sample();
    double dx = (t.x() + _x) / width - 0.5, dy = (t.y() + _y) / height - 0.5;
    return {dx, dy};
}
Vec3 Sampler::hemisphere() {
    auto v = make_pair(sample(), sample());
    double z = v.first, phi = v.second * (2 * pi) - pi;
    return {sin(phi) * sqrt(1 - z * z), cos(phi) * sqrt(1 - z * z), z};
}

Vec3 Sampler::sphere() {
    auto v = make_pair(sample(), sample());
    double z = v.first * 2 - 1, phi = v.second * (2 * pi) - pi;
    return {sin(phi) * sqrt(1 - z * z), cos(phi) * sqrt(1 - z * z), z};
}

#endif
#endif /* sampler_hpp */
