#ifndef shape_hpp
#define shape_hpp

#include "utils.hpp"
#include "geometry.hpp"

class Shape
{

protected:
    Cuboid box;

public:
    void virtual inter(const Ray &ray, int &hit, Vec3 &hitpoint) const;
    Ray virtual normal(const Ray &ray, const Vec3 &inter) const;
        // norm should be unitary

    Cuboid virtual outline() const;
};

class Sphere : public Shape
{
    Vec3 o;
    double r;

    bool contain(const Vec3 &t) const;

public:
    Sphere (Vec3 _o, double _r);

    void inter(const Ray &ray, int &hit, Vec3 &hitpoint) const override;
    Ray normal(const Ray &ray, const Vec3 &inter) const override;
};

class Flat : public Shape
{
    int n;
    vector<Vec3> vertexs, T_vertexs;
    Vec3 norm;
    Mat3 T;

    int is_right(const Vec3 &p1, const Vec3 &p2, const Vec3 &q) const;

public:
    Flat (int _n, ...);

    void inter(const Ray &ray, int &hit, Vec3 &hitpoint) const override;
    Ray normal(const Ray &ray, const Vec3 &inter) const override;
};

class Disc : public Shape
{
    Vec3 o, T_o, norm;
    double r;
    Mat3 T;

public:
    Disc (Vec3 _o, Vec3 _norm, double _r);

    void inter(const Ray &ray, int &hit, Vec3 &hitpoint) const override;
    Ray normal(const Ray &ray, const Vec3 &inter) const override;
};

class TriObj : public Shape
{
    Vec3 V0, V1, V2, norm;

public:
    TriObj (Vec3 _V0, Vec3 _V1, Vec3 _V2);

    void inter(const Ray &ray, int &hit, Vec3 &hitpoint) const override;
    Ray normal(const Ray &ray, const Vec3 &inter) const override;
};


#ifdef library

void Shape::inter(const Ray &ray, int &hit, Vec3 &hitpoint) const
{
    throw "NotImplementedError";
}
Ray Shape::normal(const Ray &ray, const Vec3 &inter) const
{
    throw "NotImplementedError";
}

Cuboid Shape::outline() const
{
    return box;
}


Sphere::Sphere (Vec3 _o, double _r)
{
    o = _o;
    r = _r;
    box = Cuboid(o - Vec3(r, r, r), o + Vec3(r, r, r));
}

bool Sphere::contain(const Vec3 &t) const
{
    return (o - t).norm2() <= r * r + r * EPS;
}

void Sphere::inter(const Ray &ray, int &hit, Vec3 &hitpoint) const
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
Ray Sphere::normal(const Ray &ray, const Vec3 &inter) const
{
    return Ray(inter, (inter - o).scale(1));
}



Flat::Flat (int _n, ...)
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

int Flat::is_right(const Vec3 &p1, const Vec3 &p2, const Vec3 &q) const
{
    if (p1.d[0] > p2.d[0]) return is_right(p2, p1, q);
    if (q.d[0] < p1.d[0]) return 0;
    if (p2.d[0] <= q.d[0]) return 0;
    return (q.d[1] - p1.d[1]) * (p2.d[0] - p1.d[0])
        > (q.d[0] - p1.d[0]) * (p2.d[1] - p1.d[1]);
}

void Flat::inter(const Ray &ray, int &hit, Vec3 &hitpoint) const
{
    Ray T_ray = Ray(T * ray.o, T * ray.d);
    if (abs(T_ray.d.d[2]) < EPS) { hit = 0; return; }

    double dis = (T_vertexs[0].d[2] - T_ray.o.d[2]) / T_ray.d.d[2];
    if (dis < 0) { hit = 0; return; }
    Vec3 T_candi = T_ray.o + T_ray.d * dis;

    int inside = is_right(T_vertexs[0], T_vertexs[n - 1], T_candi);
    for (int i = 0; i + 1 < n; ++ i)
        inside ^= is_right(T_vertexs[i], T_vertexs[i + 1], T_candi);

    if (!inside) { hit = 0; return; }
    hit = 1; hitpoint = ray.o + ray.d * dis;
}
Ray Flat::normal(const Ray &ray, const Vec3 &inter) const
{
    return Ray(inter, norm);
}


Disc::Disc (Vec3 _o, Vec3 _norm, double _r)
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

void Disc::inter(const Ray &ray, int &hit, Vec3 &hitpoint) const
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
Ray Disc::normal(const Ray &ray, const Vec3 &inter) const
{
    return Ray(inter, norm);
}

TriObj::TriObj (Vec3 _V0, Vec3 _V1, Vec3 _V2)
{
    V0 = _V0;
    V1 = _V1;
    V2 = _V2;
    norm = ((V1 - V0) ^ (V2 - V0)).scale(1);
    box = Cuboid(V0) + Cuboid(V1) + Cuboid(V2);
}

void TriObj::inter(const Ray &ray, int &hit, Vec3 &hitpoint) const
{
    Vec3 E1 = V1 - V0, E2 = V2 - V0, T = ray.o - V0;
    Vec3 P = ray.d ^ E2;
    double det = P * E1;
    if (det < 0) {det = -det; T = -T;}
    if (det < EPS) { hit = 0; return; }
    double u = P * T;
    if (u < 0 || u > det) { hit = 0; return; }
    Vec3 Q = T ^ E1;
    double t = Q * E2;
    if (t < 0) { hit = 0; return; }

    double v = Q * ray.d;
    if (v < 0 || u + v > det) { hit = 0; return; }
    hit = 1; hitpoint = ray.o + ray.d * (t / det);
}
Ray TriObj::normal(const Ray &ray, const Vec3 &inter) const
{
    return Ray(inter, norm);
}

#endif
#endif /* shape_hpp */
