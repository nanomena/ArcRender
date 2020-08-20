#ifndef shapes_flat_hpp
#define shapes_flat_hpp

#include "../shape.hpp"

class Flat : public Shape
{
    int n;
    vector<Vec3> vertexs, T_vertexs;
    Vec3 norm;
    Mat3 T;

    unsigned int is_right(const Vec3 &p1, const Vec3 &p2, const Vec3 &q) const;

public:
    explicit Flat(const vector<Vec3> &_vertexs);

    template<typename... T>
    explicit Flat(T... _vertexs)
    {
        vector<Vec3> tmp;
        n = 0;
        (tmp.push_back(_vertexs), ...);
        new(this)Flat(tmp);
    }

    void inter(const Ray &ray, int &is_inter, Vec3 &intersect) const override;
    Vec3 normal(const Vec3 &inter) const override;
    void sample(Vec3 &pos, double &pdf) const override;
};

#ifdef ARC_IMPLEMENTATION

Flat::Flat(const vector<Vec3> &_vertexs)
{
    n = _vertexs.size();
    vertexs = _vertexs;

    norm = ((vertexs[1] - vertexs[0]) ^ (vertexs[2] - vertexs[0])).scale(1);
    T = axis(vertexs[1] - vertexs[0], vertexs[2] - vertexs[0], norm);

    T_vertexs.resize(n);
    for (int i = 0; i < n; ++i)
        T_vertexs[i] = T * vertexs[i];
    for (int i = 0; i < n; ++i)
        box = box + Cuboid(vertexs[i]);
}

unsigned int Flat::is_right(const Vec3 &p1, const Vec3 &p2, const Vec3 &q) const
{
    if (p1.d[0] > p2.d[0]) return is_right(p2, p1, q);
    if (q.d[0] < p1.d[0]) return 0;
    if (p2.d[0] <= q.d[0]) return 0;
    return (q.d[1] - p1.d[1]) * (p2.d[0] - p1.d[0])
        > (q.d[0] - p1.d[0]) * (p2.d[1] - p1.d[1]);
}

void Flat::inter(const Ray &ray, int &is_inter, Vec3 &intersect) const
{
    Ray T_ray = Ray(T * ray.o, T * ray.d);
    if (abs(T_ray.d.d[2]) < EPS)
    {
        is_inter = 0;
        return;
    }

    double dis = (T_vertexs[0].d[2] - T_ray.o.d[2]) / T_ray.d.d[2];
    if (dis < 0)
    {
        is_inter = 0;
        return;
    }
    Vec3 T_candi = T_ray.o + T_ray.d * dis;

    unsigned int inside = is_right(T_vertexs[0], T_vertexs[n - 1], T_candi);
    for (int i = 0; i + 1 < n; ++i)
        inside ^= is_right(T_vertexs[i], T_vertexs[i + 1], T_candi);

    if (!inside)
    {
        is_inter = 0;
        return;
    }
    is_inter = 1;
    intersect = ray.o + ray.d * dis;
}
Vec3 Flat::normal(const Vec3 &inter) const
{
    return norm;
}
void Flat::sample(Vec3 &pos, double &pdf) const // @TODO better locate method
{
    double area = 0;
    for (int i = 2; i < n; ++ i)
        area += ((vertexs[i - 1] - vertexs[0]) ^ (vertexs[i] - vertexs[0])).norm() / 2;
    double p = RD.rand(); int k = -1;
    for (int i = 2; i < n; ++ i)
    {
        double w = ((vertexs[i - 1] - vertexs[0]) ^ (vertexs[i] - vertexs[0])).norm() / 2;
        if (p < w / area) { k = i; break; }
        else p -= w / area;
    }
    double a = RD.rand(), b = RD.rand();
    if (a + b > 1) a = 1 - a, b = 1 - b;
    pos = (vertexs[k - 1] - vertexs[0]) * a + (vertexs[k] - vertexs[0]) * b + vertexs[0];
    pdf = 1 / area;
}

#endif
#endif /* shapes_flat_hpp */