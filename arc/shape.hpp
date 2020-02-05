#ifndef SHAPE
#define SHAPE
#include <cmath>
#include <vector>
#include <stdarg.h>
using namespace std;
#include "geo.hpp"

class Shape
{

public:
    Shape () {}
    void virtual inter(const Ray &ray, int &hit, Vec3 &hitpoint) const { throw "NotImplementedError"; }
    Ray virtual normal(const Ray &ray, const Vec3 &inter) const { throw "NotImplementedError"; }
        // norm should be unitary

    Cuboid box;
    Cuboid virtual outline() const { return box; }
};

class Sphere : public Shape
{
    Vec3 o; double r;

public:
    Sphere (Vec3 _o, double _r)
    {
        o = _o;
        r = _r;
        box = Cuboid(o - Vec3(r, r, r), o + Vec3(r, r, r));
    }

    bool contain(const Vec3 &t) const { return (o - t).norm2() <= r * r; }
    void inter(const Ray &ray, int &hit, Vec3 &hitpoint) const
    {
        Vec3 x = (o - ray.o).project(ray.d), y = (o - ray.o) - x;
        double d2 = r * r - y.norm2();
        if (d2 < 0) { hit = 0; return; }
        Vec3 chord = ray.d.scale(sqrt(d2));
        if (contain(ray.o)) { hit = 1; hitpoint = ray.o + x + chord; }
        else
        {
            Vec3 candi = ray.o + x - chord;
            if ((candi - ray.o) * ray.d <= 0) { hit = 0; return; }
            hit = 1; hitpoint = candi;
        }
    }
    Ray normal(const Ray &ray, const Vec3 &inter) const
    {
        return Ray(inter, (inter - o).scale(1));
    }
};

int is_right(const Vec3 &p1, const Vec3 &p2, const Vec3 &q)
{
    if (p1.d[0] > p2.d[0]) return is_right(p2, p1, q);
    if (q.d[0] < p1.d[0]) return 0;
    if (p2.d[0] <= q.d[0]) return 0;
    return (q.d[1] - p1.d[1]) * (p2.d[0] - p1.d[0])
        > (q.d[0] - p1.d[0]) * (p2.d[1] - p1.d[1]);
}

class Flat : public Shape
{
    int n;
    vector<Vec3> vertexs, T_vertexs;
    Vec3 norm;
    Mat3 T;

public:
    Flat (int _n, ...)
    {
        n = _n;
        va_list p;
        va_start(p, _n);
        for (int i = 0; i < n; ++ i)
            vertexs.push_back(va_arg(p, Vec3));

        norm = ((vertexs[1] - vertexs[0]) ^ (vertexs[2] - vertexs[0])).scale(1);
        T = axis(vertexs[1] - vertexs[0], vertexs[2] - vertexs[0], norm);

        T_vertexs.resize(n);
        for (int i = 0; i < n; ++ i)
            T_vertexs[i] = T * vertexs[i];
        for (int i = 0; i < n; ++ i)
            box = box + Cuboid(vertexs[i]);
    }

    void inter(const Ray &ray, int &hit, Vec3 &hitpoint) const
    {
        Ray T_ray = Ray(T * ray.o, T * ray.d);
        if (abs(T_ray.d.d[2]) < EPS) { hit = 0; return; }

        double dis = (T_vertexs[0].d[2] - T_ray.o.d[2]) / T_ray.d.d[2];
        if (dis < 0) { hit = 0; return; }
        Vec3 T_candi = T_ray.o + T_ray.d * dis;

        int inside = is_right(T_vertexs[0], T_vertexs[n - 1], T_candi);
        for (int i = 0; i + 1 < T_vertexs.size(); ++ i)
            inside ^= is_right(T_vertexs[i], T_vertexs[i + 1], T_candi);

        if (!inside) { hit = 0; return; }
        hit = 1; hitpoint = ray.o + ray.d * dis;
    }
    Ray normal(const Ray &ray, const Vec3 &inter) const
    {
        return Ray(inter, norm);
    }
};

class Disc : public Shape
{
    Vec3 o, T_o, norm; double r;
    Mat3 T;


public:
    Disc (Vec3 _o, Vec3 _norm, double _r)
    {
        o = _o; norm = _norm.scale(1); r = _r;

        Vec3 x, y, z = norm;

        if (z.d[0] * z.d[0] + z.d[1] * z.d[1] > EPS)
            x = Vec3(z.d[1], -z.d[0], 0).scale(1);
        else
            x = Vec3(1, 0, 0);
        y = (x ^ z).scale(1);

        T = axis(x, y, z);
        T_o = T * o;

        Vec3 p(
            (Vec3(1, 0, 0) ^ norm).norm(),
            (Vec3(0, 1, 0) ^ norm).norm(),
            (Vec3(0, 0, 1) ^ norm).norm()
        );
        box = Cuboid(o - p * r, o + p * r);
    }

    void inter(const Ray &ray, int &hit, Vec3 &hitpoint) const
    {
        Ray T_ray = Ray(T * ray.o, T * ray.d);
        if (abs(T_ray.d.d[2]) < EPS) { hit = 0; return; }

        double dis = (T_o.d[2] - T_ray.o.d[2]) / T_ray.d.d[2];
        if (dis < 0) { hit = 0; return; }
        Vec3 T_candi = T_ray.o + T_ray.d * dis;

        int inside = ((T_candi - T_o).norm2() < r * r);

        if (!inside) { hit = 0; return; }
        hit = 1; hitpoint = ray.o + ray.d * dis;
    }
    Ray normal(const Ray &ray, const Vec3 &inter) const
    {
        return Ray(inter, norm);
    }
};

#endif
