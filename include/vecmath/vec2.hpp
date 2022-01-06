#ifndef ARC_INCLUDE_VECMATH_VEC2_HPP
#define ARC_INCLUDE_VECMATH_VEC2_HPP

class Vec2 {
public:
    Vec2();
    explicit Vec2(const double d[]);
    Vec2(double x, double y);

    double x() const;
    double y() const;
    const double &operator [](int i) const;
    double &operator [](int i);

    Vec2 &operator +=(const Vec2 &v);
    Vec2 &operator -=(const Vec2 &v);
    Vec2 &operator *=(double f);
    Vec2 &operator /=(double f);

    double squaredLength() const;
    double length() const;
    Vec2 norm(double f = 1) const;

protected:
    double d[2];
} __attribute__((aligned(16)));

Vec2 operator -(const Vec2 &v);
Vec2 operator +(const Vec2 &v0, const Vec2 &v1);
Vec2 operator -(const Vec2 &v0, const Vec2 &v1);
double operator *(const Vec2 &v0, const Vec2 &v1);
Vec2 operator *(double f, const Vec2 &v);
Vec2 operator *(const Vec2 &v, double f);
Vec2 operator /(const Vec2 &v, double f);

ostream& operator <<(ostream &o, const Vec2 &v);

#ifdef ARC_IMPLEMENTATION

Vec2::Vec2() : d{0, 0} {}
Vec2::Vec2(const double d[]) : d{d[0], d[1]} {}
Vec2::Vec2(double x, double y) : d{x, y} {}

double Vec2::x() const { return d[0]; }
double Vec2::y() const { return d[1]; }
const double &Vec2::operator [](int i) const { return d[i]; }
double &Vec2::operator [](int i) { return d[i]; }

Vec2 &Vec2::operator +=(const Vec2 &v) { return *this = *this + v; }
Vec2 &Vec2::operator -=(const Vec2 &v) { return *this = *this - v; }
Vec2 &Vec2::operator *=(double f) { return *this = *this * f; }
Vec2 &Vec2::operator /=(double f) { return *this = *this / f; }

Vec2 operator -(const Vec2 &v) { return {-v.x(), -v.y()}; }
Vec2 operator +(const Vec2 &v0, const Vec2 &v1) { return {v0.x() + v1.x(), v0.y() + v1.y()}; }
Vec2 operator -(const Vec2 &v0, const Vec2 &v1) { return {v0.x() - v1.x(), v0.y() - v1.y()}; }
double operator *(const Vec2 &v0, const Vec2 &v1) { return v0.x() * v1.x() + v0.y() * v1.y(); }
Vec2 operator *(double f, const Vec2 &v) { return {v.x() * f, v.y() * f}; }
Vec2 operator *(const Vec2 &v, double f) { return {v.x() * f, v.y() * f}; }
Vec2 operator /(const Vec2 &v, double f) { return {v.x() / f, v.y() / f}; }

double Vec2::squaredLength() const { return (*this) * (*this); }
double Vec2::length() const { return sqrt(squaredLength()); }
Vec2 Vec2::norm(double f) const { return (*this) / length() * f; }

ostream& operator <<(ostream &o, const Vec2 &v) {
    o << "[" << v.x() << "," << v.y() << "]";
    return o;
}

#endif
#endif //ARC_INCLUDE_VECMATH_VEC2_HPP
