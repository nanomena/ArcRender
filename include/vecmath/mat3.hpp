#ifndef ARC_INCLUDE_VECMATH_MAT3_HPP
#define ARC_INCLUDE_VECMATH_MAT3_HPP

class Vec3;

class Mat3 {
public:
    Mat3();
    explicit Mat3(const double d[]);
    Mat3(
        double m00, double m01, double m02,
        double m10, double m11, double m12,
        double m20, double m21, double m22
    );

    const double &operator ()(int i, int j) const;
    double &operator ()(int i, int j);

    Mat3 &operator *=(const Mat3 &m);

    Mat3 T() const;
    double det() const;
    Mat3 I() const;

protected:
    double d[9]; // Row major!
};

Mat3 operator *(const Mat3 &m0, const Mat3 &m1);
Vec3 operator *(const Mat3 &m0, const Vec3 &v1);

ostream &operator <<(ostream &o, const Mat3 &v);

Mat3 makeAxisInv(const Vec3 &x, const Vec3 &y, const Vec3 &z);
Mat3 makeAxis(const Vec3 &x, const Vec3 &y, const Vec3 &z);

#ifdef ARC_IMPLEMENTATION

Mat3::Mat3() : d{0, 0, 0, 0, 0, 0, 0, 0, 0} {}
Mat3::Mat3(const double d[]) : d{d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8]} {}
Mat3::Mat3(
    double m00, double m01, double m02,
    double m10, double m11, double m12,
    double m20, double m21, double m22
) : d{m00, m01, m02, m10, m11, m12, m20, m21, m22} {}

const double &Mat3::operator ()(int i, int j) const { return d[i * 3 + j]; }
double &Mat3::operator ()(int i, int j) { return d[i * 3 + j]; }

Mat3 &Mat3::operator *=(const Mat3 &m) { return (*this) = (*this) * m; }

Mat3 operator *(const Mat3 &m0, const Mat3 &m1) {
    Mat3 m2;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
#pragma unroll
            for (int k = 0; k < 3; ++k)
                m2(i, j) += m0(i, k) * m1(k, j);
    return m2;
}
Vec3 operator *(const Mat3 &m0, const Vec3 &v1) {
    Vec3 m2;
    for (int i = 0; i < 3; ++i)
#pragma unroll
        for (int j = 0; j < 3; ++j)
            m2[i] += m0(i, j) * v1[j];
    return m2;
}

Mat3 Mat3::T() const {
    Mat3 m;
    for (int i = 0; i < 3; ++i)
#pragma unroll
        for (int j = 0; j < 3; ++j)
            m(i, j) = (*this)(j, i);
    return m;
}
double Mat3::det() const {
    return (*this)(0, 0) * (*this)(1, 1) * (*this)(2, 2)
        - (*this)(0, 0) * (*this)(1, 2) * (*this)(2, 1)
        - (*this)(0, 1) * (*this)(1, 0) * (*this)(2, 2)
        + (*this)(0, 1) * (*this)(1, 2) * (*this)(2, 0)
        + (*this)(0, 2) * (*this)(1, 0) * (*this)(2, 1)
        - (*this)(0, 2) * (*this)(1, 1) * (*this)(2, 0);
}
Mat3 Mat3::I() const {
    double p = 1 / det();
    Mat3 m;
    m(0, 0) = ((*this)(1, 1) * (*this)(2, 2) - (*this)(1, 2) * (*this)(2, 1)) * p;
    m(0, 1) = ((*this)(0, 1) * (*this)(2, 2) - (*this)(0, 2) * (*this)(2, 1)) * -p;
    m(0, 2) = ((*this)(0, 1) * (*this)(1, 2) - (*this)(0, 2) * (*this)(1, 1)) * p;

    m(1, 0) = ((*this)(1, 0) * (*this)(2, 2) - (*this)(1, 2) * (*this)(2, 0)) * -p;
    m(1, 1) = ((*this)(0, 0) * (*this)(2, 2) - (*this)(0, 2) * (*this)(2, 0)) * p;
    m(1, 2) = ((*this)(0, 0) * (*this)(1, 2) - (*this)(0, 2) * (*this)(1, 0)) * -p;

    m(2, 0) = ((*this)(1, 0) * (*this)(2, 1) - (*this)(1, 1) * (*this)(2, 0)) * p;
    m(2, 1) = ((*this)(0, 0) * (*this)(2, 1) - (*this)(0, 1) * (*this)(2, 0)) * -p;
    m(2, 2) = ((*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0)) * p;
    return m;
}

ostream &operator <<(ostream &o, const Mat3 &v) {
    for (int i = 0; i < 3; ++i) {
        o << (i == 0 ? "[[" : " [");
        for (int j = 0; j < 3; ++j)
            o << v(i, j) << (j < 2 ? "," : "]");
        o << (i == 2 ? "]" : ",") << endl;
    }
    return o;
}

Mat3 makeAxisInv(const Vec3 &x, const Vec3 &y, const Vec3 &z) {
    assert(abs(1 - x.length()) < EPS);
    assert(abs(1 - y.length()) < EPS);
    assert(abs(1 - z.length()) < EPS);
    assert(abs(x * y) < EPS);
    assert(abs(x * z) < EPS);
    return {x[0], y[0], z[0], x[1], y[1], z[1], x[2], y[2], z[2]};
}
Mat3 makeAxis(const Vec3 &x, const Vec3 &y, const Vec3 &z) {
    return makeAxisInv(x, y, z).I();
}

void rotateAxis(const Vec3 &n, const Vec3 &v, Mat3 &T, Mat3 &TInv) {
    assert(abs(1 - n.length()) < EPS);
    Vec3 x, y, z = n;
//    cerr << "rotateAxis " << n << " " << v << endl;

    if ((v ^ n).length() > EPS)
        x = v.vert(n).norm();
    else if (sqrt(z[0] * z[0] + z[1] * z[1]) > EPS)
        x = Vec3(z[1], -z[0], 0).norm();
    else
        x = Vec3(1, 0, 0);

    y = (x ^ z).norm();

//    cerr << n << " " << v << " " << x << " " << y <<  " " << z << endl;
    TInv = makeAxisInv(x, y, z);
    T = TInv.I();
}

#endif
#endif //ARC_INCLUDE_VECMATH_MAT3_HPP
