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
    Vec3 virtual inter(const Ray &ray) const { throw "NotImplementedError"; }
    Ray virtual normal(const Ray &ray, const Vec3 &inter) const { throw "NotImplementedError"; }
};

class Sphere : public Shape
{
    Vec3 o; double r;

public:
    Sphere (Vec3 _o, double _r) { o = _o; r = _r; }

    bool contain(const Vec3 &t) const { return (o - t).norm2() <= r * r; }
    Vec3 inter(const Ray &ray) const
    {
        Vec3 x = (o - ray.o).project(ray.d), y = (o - ray.o) - x;
        double d2 = r * r - y.norm2();
        if (d2 < 0) return ray.o + ray.d.scale(INF);
        Vec3 chord = ray.d.scale(sqrt(d2));
        if (contain(ray.o)) return ray.o + x + chord;
        else
        {
            Vec3 candi = ray.o + x - chord;
            if ((candi - ray.o) * ray.d > 0) return candi;
            else return ray.o + ray.d.scale(INF);
        }
    }
    Ray normal(const Ray &ray, const Vec3 &inter) const
    {
        return Ray(inter, o - inter);
    }
};

int is_right(Vec3 p1, Vec3 p2, Vec3 q)
{
    if (p1.d[0] > p2.d[0]) swap(p1, p2);
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

        norm = (vertexs[1] - vertexs[0]) ^ (vertexs[2] - vertexs[0]);
        T = axis(vertexs[1] - vertexs[0], vertexs[2] - vertexs[0], norm);

        T_vertexs.resize(n);
        for (int i = 0; i < n; ++ i)
            T_vertexs[i] = T * vertexs[i];
    }

    Vec3 inter(const Ray &ray) const
    {
        Ray T_ray = Ray(T * ray.o, T * ray.d);
        if (abs(T_ray.d.d[2]) < EPS) return ray.o + ray.d.scale(INF);

        double dis = (T_vertexs[0].d[2] - T_ray.o.d[2]) / T_ray.d.d[2];
        if (dis < 0) return ray.o + ray.d.scale(INF);
        Vec3 T_candi = T_ray.o + T_ray.d * dis;

        int inside = is_right(T_vertexs[0], T_vertexs[n - 1], T_candi);
        for (int i = 0; i + 1 < T_vertexs.size(); ++ i)
            inside ^= is_right(T_vertexs[i], T_vertexs[i + 1], T_candi);

        if (!inside) return ray.o + ray.d.scale(INF);
        return ray.o + ray.d * dis;
    }
    Ray normal(const Ray &ray, const Vec3 &inter) const
    {
        return Ray(inter, norm);
    }
};
#endif
