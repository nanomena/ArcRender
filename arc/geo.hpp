#ifndef GEO
#define GEO
#include <cmath>
#include <iostream>
using namespace std;

const double INF = 1e50;
const double EPS = 1e-6;
const double pi = acos(-1);

struct Mat3;

struct Vec3
{
    double d[3];

    Vec3 () { d[0] = d[1] = d[2] = 0; }
    Vec3 (double _d[]) { d[0] = _d[0], d[1] = _d[1], d[2] = _d[2]; }
    Vec3 (double x, double y, double z) { d[0] = x, d[1] = y, d[2] = z; }
    void load(double _d[]) const { _d[0] = d[0], _d[1] = d[1], _d[2] = d[2];}

    Vec3 operator - () const { return Vec3(-d[0], -d[1], -d[2]); }
    Vec3 operator + (const Vec3 &t) const { return Vec3(d[0] + t.d[0], d[1] + t.d[1], d[2] + t.d[2]); }
    Vec3 operator - (const Vec3 &t) const { return Vec3(d[0] - t.d[0], d[1] - t.d[1], d[2] - t.d[2]); }
    Vec3 operator * (double t) const { return Vec3(d[0] * t, d[1] * t, d[2] * t); }
    Vec3 operator / (double t) const { return Vec3(d[0] / t, d[1] / t, d[2] / t); }
    double operator * (const Vec3 &t) const { return d[0] * t.d[0] + d[1] * t.d[1] + d[2] * t.d[2]; }
    Vec3 operator ^ (const Vec3 &t) const
    {
        return Vec3(
            d[1] * t.d[2] - d[2] * t.d[1],
            -(d[0] * t.d[2] - d[2] * t.d[0]),
            d[0] * t.d[1] - d[1] * t.d[0]
        );
    }
    double norm2() const { return (*this) * (*this); }
    double norm() const { return sqrt(norm2()); }
    Vec3 scale(double k) const { return (*this) / norm() * k; }
    Vec3 project(const Vec3 &t) const { return t / t.norm2() * (*this * t); }
    Vec3 vertical(const Vec3 &t) const { return (*this) - project(t); }
    Vec3 apply(const Mat3 &) const;

    friend ostream& operator << (ostream &s, Vec3 t)
    {
        s << "(" << t.d[0] << "," << t.d[1] << "," << t.d[2] << ")";
        return s;
    }
};

double cross(const Vec3 &a, const Vec3 &b) { return a.d[0] * b.d[1] - a.d[1] * b.d[0]; }

struct Ray
{
    Vec3 o, d;

    Ray () {}
    Ray (Vec3 _o, Vec3 _d) { o = _o, d = _d; }

    Ray move(double length) const { return Ray(o + d.scale(length), d); }
    Ray reflect(Ray n) const { return Ray(n.o, d - d.project(n.d) * 2); }
    Ray operator - () const { return Ray(o, -d); }

    friend ostream& operator << (ostream &s, Ray t)
    {
        s << t.o << "+k" << t.d;
        return s;
    }
};

struct Plane
{
    Vec3 o, x, y;

    Plane () {}
    Plane (Vec3 _o, Vec3 _x, Vec3 _y) { o = _o, x = _x, y = _y; }
};

struct Mat3
{
    double d[3][3];

    Mat3 ()
    {
        for (int i = 0; i < 3; ++ i)
            for (int j = 0; j < 3; ++ j)
                d[i][j] = 0;
    }
    Mat3 (double _d[][3])
    {
        for (int i = 0; i < 3; ++ i)
            for (int j = 0; j < 3; ++ j)
                d[i][j] = _d[i][j];
    }

    Mat3 operator + (const Mat3 &t) const
    {
        double v[3][3];
        for (int i = 0; i < 3; ++ i)
            for (int j = 0; j < 3; ++ j)
                v[i][j] = d[i][j] + t.d[i][j];
        return Mat3(v);
    }
    Mat3 operator - (const Mat3 &t) const
    {
        double v[3][3];
        for (int i = 0; i < 3; ++ i)
            for (int j = 0; j < 3; ++ j)
                v[i][j] = d[i][j] - t.d[i][j];
        return Mat3(v);
    }
    Mat3 operator * (const Mat3 &t) const
    {
        double v[3][3];
        for (int i = 0; i < 3; ++ i)
            for (int j = 0; j < 3; ++ j)
            {
                v[i][j] = 0;
                for (int k = 0; k < 3; ++ k)
                    v[i][j] += d[i][k] * t.d[k][j];
            }
        return Mat3(v);
    }
    Vec3 operator * (const Vec3 &t) const
    {
        double v[3];
        for (int i = 0; i < 3; ++ i)
        {
            v[i] = 0;
            for (int j = 0; j < 3; ++ j)
                v[i] += d[i][j] * t.d[j];
        }
        return Vec3(v);
    }
    Mat3 T() const
    {
        double v[3][3];
        for (int i = 0; i < 3; ++ i)
            for (int j = 0; j < 3; ++ j)
                v[i][j] = d[j][i];
        return Mat3(v);
    }
    double det() const
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
    Mat3 I() const
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
};

Vec3 Vec3::apply(const Mat3 &p) const
{
    double t[3];
    for (int i = 0; i < 3; ++ i)
    {
        t[i] = 0;
        for (int j = 0; j < 3; ++ j)
            t[i] += d[i] * p.d[i][j];
    }
    return Vec3(t);
}

Mat3 axis_I(const Vec3 &x, const Vec3 &y, const Vec3 &z)
{
    double v[3][3];
    v[0][0] = x.d[0], v[0][1] = y.d[0], v[0][2] = z.d[0];
    v[1][0] = x.d[1], v[1][1] = y.d[1], v[1][2] = z.d[1];
    v[2][0] = x.d[2], v[2][1] = y.d[2], v[2][2] = z.d[2];
    return Mat3(v);
}

Mat3 axis(const Vec3 &x, const Vec3 &y, const Vec3 &z)
{
    return axis_I(x, y, z).I();
}

#endif
