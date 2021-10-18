#ifndef ARC_INCLUDE_VECMATH_VEC3_HPP
#define ARC_INCLUDE_VECMATH_VEC3_HPP

class Vec3 {
public:
    Vec3();
    explicit Vec3(const double d[]);
    Vec3(double x, double y, double z);

    double x() const;
    double y() const;
    double z() const;
    const double &operator [](int i) const;
    double &operator [](int i);

    Vec3 &operator +=(const Vec3 &v);
    Vec3 &operator -=(const Vec3 &v);
    Vec3 &operator *=(double f);
    Vec3 &operator /=(double f);

    double squaredLength() const;
    double length() const;
    Vec3 norm(double f = 1) const;
    Vec3 proj(const Vec3 &v) const;
    Vec3 vert(const Vec3 &v) const;

protected:
    double d[3];
} __attribute__((aligned(32)));

Vec3 operator -(const Vec3 &v);
Vec3 operator +(const Vec3 &v0, const Vec3 &v1);
Vec3 operator -(const Vec3 &v0, const Vec3 &v1);
double operator *(const Vec3 &v0, const Vec3 &v1);
Vec3 operator *(double f, const Vec3 &v);
Vec3 operator *(const Vec3 &v, double f);
Vec3 operator /(const Vec3 &v, double f);
Vec3 operator ^(const Vec3 &v0, const Vec3 &v1);

ostream& operator <<(ostream &o, const Vec3 &v);

#ifdef ARC_IMPLEMENTATION

Vec3::Vec3() : d{0, 0, 0} {}
Vec3::Vec3(const double d[]) : d{d[0], d[1], d[2]} {}
Vec3::Vec3(double x, double y, double z) : d{x, y, z} {}

double Vec3::x() const { return d[0]; }
double Vec3::y() const { return d[1]; }
double Vec3::z() const { return d[2]; }
const double &Vec3::operator [](int i) const { return d[i]; }
double &Vec3::operator [](int i) { return d[i]; }

Vec3 &Vec3::operator +=(const Vec3 &v) { return *this = *this + v; }
Vec3 &Vec3::operator -=(const Vec3 &v) { return *this = *this - v; }
Vec3 &Vec3::operator *=(double f) { return *this = *this * f; }
Vec3 &Vec3::operator /=(double f) { return *this = *this / f; }

Vec3 operator -(const Vec3 &v) { return {-v.x(), -v.y(), -v.z()}; }
Vec3 operator +(const Vec3 &v0, const Vec3 &v1) { return {v0.x() + v1.x(), v0.y() + v1.y(), v0.z() + v1.z()}; }
Vec3 operator -(const Vec3 &v0, const Vec3 &v1) { return {v0.x() - v1.x(), v0.y() - v1.y(), v0.z() - v1.z()}; }
double operator *(const Vec3 &v0, const Vec3 &v1) { return v0.x() * v1.x() + v0.y() * v1.y() + v0.z() * v1.z(); }
Vec3 operator *(double f, const Vec3 &v) { return {v.x() * f, v.y() * f, v.z() * f}; }
Vec3 operator *(const Vec3 &v, double f) { return {v.x() * f, v.y() * f, v.z() * f}; }
Vec3 operator /(const Vec3 &v, double f) { return {v.x() / f, v.y() / f, v.z() / f}; }
Vec3 operator ^(const Vec3 &v0, const Vec3 &v1) {
    return {
        v0[1] * v1[2] - v0[2] * v1[1],
        -(v0[0] * v1[2] - v0[2] * v1[0]),
        v0[0] * v1[1] - v0[1] * v1[0]
    };
}

double Vec3::squaredLength() const { return (*this) * (*this); }
double Vec3::length() const { return sqrt(squaredLength()); }
Vec3 Vec3::norm(double f) const { return (*this) / length() * f; }
Vec3 Vec3::proj(const Vec3 &v) const { return v / v.squaredLength() * ((*this) * v); }
Vec3 Vec3::vert(const Vec3 &v) const { return (*this) - proj(v); }

ostream& operator <<(ostream &o, const Vec3 &v) {
    o << "[" << v.x() << "," << v.y() << "," << v.z() << "]";
    return o;
}

#endif
#endif //ARC_INCLUDE_VECMATH_VEC3_HPP
