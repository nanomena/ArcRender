#ifndef geometry_hpp
#define geometry_hpp

#include "utils.hpp"

extern const double INF;
extern const double EPS;
extern const double pi;

struct Mat3;

struct Vec2
{
    double d[2];

    Vec2();
    explicit Vec2(double d_[]);
    Vec2(double x, double y);

    Vec2 operator -() const;
    Vec2 operator +(const Vec2 &t) const;
    Vec2 operator -(const Vec2 &t) const;
    Vec2 operator *(double t) const;
    Vec2 operator /(double t) const;
};

struct Vec3
{
    double d[3];

    Vec3();
    explicit Vec3(double d_[]);
    Vec3(double x, double y, double z);

    Vec3 operator -() const;
    Vec3 operator +(const Vec3 &t) const;
    Vec3 operator -(const Vec3 &t) const;
    Vec3 operator *(double t) const;
    Vec3 operator /(double t) const;
    double operator *(const Vec3 &t) const;
    Vec3 operator ^(const Vec3 &t) const;

    double norm2() const;
    double norm() const;
    Vec3 scale(double k) const;
    Vec3 project(const Vec3 &t) const;
    Vec3 vertical(const Vec3 &t) const;
    Vec3 apply(const Mat3 &t) const;

    friend ostream &operator <<(ostream &s, Vec3 t);
};

struct Ray
{
    Vec3 o, d;

    Ray();
    Ray(Vec3 o_, Vec3 d_);

    void move(double length);

    friend ostream &operator <<(ostream &s, Ray t);
};

struct Mat3
{
    double d[3][3];

    Mat3();
    explicit Mat3(double d_[][3]);

    Mat3 operator +(const Mat3 &t) const;
    Mat3 operator -(const Mat3 &t) const;
    Mat3 operator *(const Mat3 &t) const;
    Vec3 operator *(const Vec3 &t) const;

    Mat3 T() const;
    double det() const;
    Mat3 I() const;
};

Mat3 axis_I(const Vec3 &x, const Vec3 &y, const Vec3 &z);
Mat3 axis(const Vec3 &x, const Vec3 &y, const Vec3 &z);

struct Cuboid
{
    Vec3 n, x;

    Cuboid();
    explicit Cuboid(Vec3 p);
    Cuboid(Vec3 n_, Vec3 x_);

    Cuboid operator +(const Cuboid &t) const;
    double operator *(const Cuboid &t) const;

    void inter(const Ray &ray, int &is_inter, Vec3 &intersect) const;

    friend ostream &operator <<(ostream &s, Cuboid t);
};

struct Trans3
{
    Mat3 T;
    Vec3 O;

    Trans3();
    Trans3(Mat3 T_, Vec3 O_);
    Trans3(Vec3 x, Vec3 y, Vec3 z, Vec3 O_);
    Trans3(Vec3 V0, Vec3 V1, Vec3 V2, Vec3 Vt0, Vec3 Vt1, Vec3 Vt2);

    Vec3 apply(const Vec3 &t) const;
};

#ifdef ARC_IMPLEMENTATION

const double INF = 1e50;
const double EPS = 1e-9;
const double pi = acos(-1);

Vec2::Vec2()
{
    d[0] = d[1] = 0;
}
Vec2::Vec2(double d_[])
{
    d[0] = d_[0];
    d[1] = d_[1];
}
Vec2::Vec2(double x, double y)
{
    d[0] = x;
    d[1] = y;
}

Vec2 Vec2::operator -() const
{
    return Vec2(-d[0], -d[1]);
}
Vec2 Vec2::operator +(const Vec2 &t) const
{
    return Vec2(d[0] + t.d[0], d[1] + t.d[1]);
}
Vec2 Vec2::operator -(const Vec2 &t) const
{
    return Vec2(d[0] - t.d[0], d[1] - t.d[1]);
}
Vec2 Vec2::operator *(double t) const
{
    return Vec2(d[0] * t, d[1] * t);
}
Vec2 Vec2::operator /(double t) const
{
    return Vec2(d[0] / t, d[1] / t);
}

Vec3::Vec3()
{
    d[0] = d[1] = d[2] = 0;
}
Vec3::Vec3(double d_[])
{
    d[0] = d_[0];
    d[1] = d_[1];
    d[2] = d_[2];
}
Vec3::Vec3(double x, double y, double z)
{
    d[0] = x;
    d[1] = y;
    d[2] = z;
}

Vec3 Vec3::operator -() const
{
    return Vec3(-d[0], -d[1], -d[2]);
}
Vec3 Vec3::operator +(const Vec3 &t) const
{
    return Vec3(d[0] + t.d[0], d[1] + t.d[1], d[2] + t.d[2]);
}
Vec3 Vec3::operator -(const Vec3 &t) const
{
    return Vec3(d[0] - t.d[0], d[1] - t.d[1], d[2] - t.d[2]);
}
Vec3 Vec3::operator *(double t) const
{
    return Vec3(d[0] * t, d[1] * t, d[2] * t);
}
Vec3 Vec3::operator /(double t) const
{
    return Vec3(d[0] / t, d[1] / t, d[2] / t);
}
double Vec3::operator *(const Vec3 &t) const
{
    return d[0] * t.d[0] + d[1] * t.d[1] + d[2] * t.d[2];
}
Vec3 Vec3::operator ^(const Vec3 &t) const
{
    return Vec3(
        d[1] * t.d[2] - d[2] * t.d[1],
        -(d[0] * t.d[2] - d[2] * t.d[0]),
        d[0] * t.d[1] - d[1] * t.d[0]
    );
}

double Vec3::norm2() const
{
    return (*this) * (*this);
}
double Vec3::norm() const
{
    return sqrt(norm2());
}
Vec3 Vec3::scale(double k) const
{
    return (*this) / norm() * k;
}
Vec3 Vec3::project(const Vec3 &t) const
{
    return t / t.norm2() * (*this * t);
}
Vec3 Vec3::vertical(const Vec3 &t) const
{
    return (*this) - project(t);
}
Vec3 Vec3::apply(const Mat3 &t) const
{
    double p[3];
    for (int i = 0; i < 3; ++i)
    {
        p[i] = 0;
        for (int j = 0; j < 3; ++j)
            p[i] += d[i] * t.d[i][j];
    }
    return Vec3(p);
}

ostream &operator <<(ostream &s, Vec3 t)
{
    s << "(" << t.d[0] << "," << t.d[1] << "," << t.d[2] << ")";
    return s;
}

Ray::Ray() = default;
Ray::Ray(Vec3 o_, Vec3 d_)
{
    o = o_;
    d = d_;
}

void Ray::move(double length)
{
    o = o + d.scale(length);
}

ostream &operator <<(ostream &s, Ray t)
{
    s << t.o << "+k" << t.d;
    return s;
}

Mat3::Mat3()
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            d[i][j] = 0;
}
Mat3::Mat3(double d_[][3])
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            d[i][j] = d_[i][j];
}

Mat3 Mat3::operator +(const Mat3 &t) const
{
    double v[3][3];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            v[i][j] = d[i][j] + t.d[i][j];
    return Mat3(v);
}
Mat3 Mat3::operator -(const Mat3 &t) const
{
    double v[3][3];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            v[i][j] = d[i][j] - t.d[i][j];
    return Mat3(v);
}
Mat3 Mat3::operator *(const Mat3 &t) const
{
    double v[3][3];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            v[i][j] = 0;
            for (int k = 0; k < 3; ++k)
                v[i][j] += d[i][k] * t.d[k][j];
        }
    return Mat3(v);
}
Vec3 Mat3::operator *(const Vec3 &t) const
{
    double v[3];
    for (int i = 0; i < 3; ++i)
    {
        v[i] = 0;
        for (int j = 0; j < 3; ++j)
            v[i] += d[i][j] * t.d[j];
    }
    return Vec3(v);
}
Mat3 Mat3::T() const
{
    double v[3][3];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            v[i][j] = d[j][i];
    return Mat3(v);
}
double Mat3::det() const
{
    return (
        d[0][0] * d[1][1] * d[2][2]
            - d[0][0] * d[1][2] * d[2][1]
            - d[0][1] * d[1][0] * d[2][2]
            + d[0][1] * d[1][2] * d[2][0]
            + d[0][2] * d[1][0] * d[2][1]
            - d[0][2] * d[1][1] * d[2][0]
    );
}
Mat3 Mat3::I() const
{
    double p = 1 / det();
    double v[3][3];
    v[0][0] = (d[1][1] * d[2][2] - d[1][2] * d[2][1]) * p;
    v[1][0] = (d[1][0] * d[2][2] - d[1][2] * d[2][0]) * -p;
    v[2][0] = (d[1][0] * d[2][1] - d[1][1] * d[2][0]) * p;

    v[0][1] = (d[0][1] * d[2][2] - d[0][2] * d[2][1]) * -p;
    v[1][1] = (d[0][0] * d[2][2] - d[0][2] * d[2][0]) * p;
    v[2][1] = (d[0][0] * d[2][1] - d[0][1] * d[2][0]) * -p;

    v[0][2] = (d[0][1] * d[1][2] - d[0][2] * d[1][1]) * p;
    v[1][2] = (d[0][0] * d[1][2] - d[0][2] * d[1][0]) * -p;
    v[2][2] = (d[0][0] * d[1][1] - d[0][1] * d[1][0]) * p;
    return Mat3(v);
}

Mat3 axis_I(const Vec3 &x, const Vec3 &y, const Vec3 &z)
{
    double v[3][3];
    v[0][0] = x.d[0];
    v[0][1] = y.d[0];
    v[0][2] = z.d[0];
    v[1][0] = x.d[1];
    v[1][1] = y.d[1];
    v[1][2] = z.d[1];
    v[2][0] = x.d[2];
    v[2][1] = y.d[2];
    v[2][2] = z.d[2];
    return Mat3(v);
}
Mat3 axis(const Vec3 &x, const Vec3 &y, const Vec3 &z)
{
    return axis_I(x, y, z).I();
}

Cuboid::Cuboid()
{
    n = Vec3(INF, INF, INF);
    x = Vec3(-INF, -INF, -INF);
}
Cuboid::Cuboid(Vec3 p)
{
    n = x = p;
}
Cuboid::Cuboid(Vec3 n_, Vec3 x_)
{
    n = n_;
    x = x_;
}

Cuboid Cuboid::operator +(const Cuboid &t) const
{
    return Cuboid(
        Vec3(min(n.d[0], t.n.d[0]), min(n.d[1], t.n.d[1]), min(n.d[2], t.n.d[2])),
        Vec3(max(x.d[0], t.x.d[0]), max(x.d[1], t.x.d[1]), max(x.d[2], t.x.d[2]))
    );
}
double Cuboid::operator *(const Cuboid &t) const
{
    return max(0., min(x.d[0], t.x.d[0]) - max(n.d[0], t.n.d[0]))
        * max(0., min(x.d[1], t.x.d[1]) - max(n.d[1], t.n.d[1]))
        * max(0., min(x.d[2], t.x.d[2]) - max(n.d[2], t.n.d[2]));
}

void Cuboid::inter(const Ray &ray, int &is_inter, Vec3 &intersect) const
{
    double l = 0, r = INF;
    for (int i = 0; i < 3; ++i)
    {
        double p = (n.d[i] - ray.o.d[i]) / ray.d.d[i];
        double q = (x.d[i] - ray.o.d[i]) / ray.d.d[i];
        if (p < q)
        {
            l = max(l, p);
            r = min(r, q);
        }
        else
        {
            l = max(l, q);
            r = min(r, p);
        }
    }
    if (l > r)
    {
        is_inter = 0;
        return;
    }
    is_inter = 1;
    intersect = ray.o + ray.d * l;
}

ostream &operator <<(ostream &s, Cuboid t)
{
    s << "[" << t.n << "|" << t.x << "]";
    return s;
}

Trans3::Trans3()
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            T.d[i][j] = (i == j);
}
Trans3::Trans3(Mat3 T_, Vec3 O_)
{
    T = T_;
    O = O_;
}
Trans3::Trans3(Vec3 x, Vec3 y, Vec3 z, Vec3 O_)
{
    T = axis_I(x, y, z);
    O = O_;
}
Trans3::Trans3(Vec3 V0, Vec3 V1, Vec3 V2, Vec3 Vt0, Vec3 Vt1, Vec3 Vt2)
{
    T = axis_I(Vt1 - Vt0, Vt2 - Vt0, (Vt1 - Vt0) ^ (Vt2 - Vt0)) *
        axis(V1 - V0, V2 - V0, (V1 - V0) ^ (V2 - V0));
    O = Vt0 - T * V0;
}

Vec3 Trans3::apply(const Vec3 &t) const
{
    return T * t + O;
}

#endif
#endif /* geometry_hpp */
