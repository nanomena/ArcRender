#ifndef SAMPLER
#define SAMPLER
#include <random>
#include <cmath>
#include "geo.hpp"
using namespace std;

class Sampler
{
    unsigned seed, MAX;

public:
    Sampler ()
    {
        seed = 1;
        MAX = (1 << 28);
    }
    double rand(double l = 0, double r = 1)
    {
        seed = (0x5DEECE66DUL * seed + 0xB16) & 0xFFFFFFFFFFFFUL;
        double v = (seed >> 4) * (r - l) / MAX + l;
        // double v = 1. * ::rand() / RAND_MAX * (r - l)  + l;
        return v;
    }
    Vec3 semisphere()
    {
        double z = rand(), phi = rand(-pi, pi);
        return Vec3(sin(phi) * sqrt(1 - z * z), cos(phi) * sqrt(1 - z * z), z);
    }
    Vec3 semisphere(Vec3 z)
    {
        Vec3 x, y;
        if ((z.d[0] * z.d[0] + z.d[1] * z.d[1]) / (z.d[2] * z.d[2]) < EPS)
        {
            x = Vec3(1, 0, 0);
            y = Vec3(0, 1, 0);
        }
        else
        {
            x = Vec3(z.d[1], -z.d[0], 0);
            y = x ^ z;
        }
        Vec3 vec = semisphere();
        return x / x.norm() * vec.d[0] + y / y.norm() * vec.d[1]
            + z / z.norm() * vec.d[2];
    }
    Ray diffuse(const Ray &normal)
    {
        return Ray(normal.o, semisphere(normal.d));
    }
} RD;

#endif
