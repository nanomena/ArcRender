#ifndef ARCRENDER_INCLUDE_VECMATH_POLY_HPP
#define ARCRENDER_INCLUDE_VECMATH_POLY_HPP

class Poly {
public:
    Poly() = default;
    explicit Poly(int n);
    explicit Poly(const vector<double> &d);

    int size() const;

    double operator ()(double x) const;
    Poly operator ()(const Poly& x) const;
    const double &operator [](int i) const;
    double &operator [](int i);

    Poly &operator +=(const Poly &v);
    Poly &operator -=(const Poly &v);
    Poly &operator *=(const Poly &v);
    Poly &operator *=(double f);
    Poly &operator /=(double f);

    Poly stripe() const;

    double* coefficient();

private:
    vector<double> d;
};

template<typename... Args>
Poly GenPoly(Args... d) {
    vector<double> tmp;
    (tmp.push_back(d), ...);
    return Poly(tmp);
}

Poly operator -(const Poly &v);
Poly operator +(const Poly &v0, const Poly &v1);
Poly operator -(const Poly &v0, const Poly &v1);
Poly operator *(const Poly &v0, const Poly &v1);
Poly operator *(double f, const Poly &v);
Poly operator *(const Poly &v, double f);
Poly operator /(const Poly &v, double f);

ostream& operator <<(ostream &o, const Poly &v);

#ifdef ARC_IMPLEMENTATION

Poly::Poly(int n) { d.resize(n); }
Poly::Poly(const vector<double> &d) : d(d) {}

int Poly::size() const {
    return d.size();
}

double Poly::operator ()(double x) const {
    double v = 0;
    for (int i = size() - 1; i >= 0; -- i) v = v * x + d[i];
    return v;
}
Poly Poly::operator ()(const Poly& x) const {
    Poly v(1);
    for (int i = size() - 1; i >= 0; -- i) {
        v = v * x;
        v[0] += d[i];
    }
    return v;
}
const double &Poly::operator [](int i) const { return d[i]; }
double &Poly::operator [](int i) { return d[i]; }

Poly &Poly::operator +=(const Poly &v) { return *this = *this + v; }
Poly &Poly::operator -=(const Poly &v) { return *this = *this - v; }
Poly &Poly::operator *=(const Poly &v) { return *this = *this * v; }
Poly &Poly::operator *=(double f) { return *this = *this * f; }
Poly &Poly::operator /=(double f) { return *this = *this / f; }

Poly operator -(const Poly &v) {
    Poly w = Poly(v.size());
    for (int i = 0; i < w.size(); ++ i) w[i] = -v[i];
    return w;
}

Poly operator +(const Poly &v0, const Poly &v1) {
    Poly w = Poly(max(v0.size(), v1.size()));
    for (int i = 0; i < w.size(); ++ i) w[i] = (i < v0.size() ? v0[i] : 0) + (i < v1.size() ? v1[i] : 0);
    return w;
}

Poly operator -(const Poly &v0, const Poly &v1) {
    Poly w = Poly(max(v0.size(), v1.size()));
    for (int i = 0; i < w.size(); ++ i) w[i] = (i < v0.size() ? v0[i] : 0) - (i < v1.size() ? v1[i] : 0);
    return w;
}

Poly operator *(const Poly &v0, const Poly &v1) {
    Poly w = Poly(v0.size() + v1.size() - 1);
    for (int i0 = 0; i0 < v0.size(); ++ i0)
        for (int i1 = 0; i1 < v1.size(); ++ i1)
            w[i0 + i1] += v0[i0] * v1[i1];
    return w;
}

Poly operator *(const Poly &v, double f) {
    Poly w = Poly(v.size());
    for (int i = 0; i < w.size(); ++ i) w[i] = v[i] * f;
    return w;
}

Poly operator *(double f, const Poly &v) {
    Poly w = Poly(v.size());
    for (int i = 0; i < w.size(); ++ i) w[i] = v[i] * f;
    return w;
}

Poly operator /(const Poly &v, double f) {
    Poly w = Poly(v.size());
    for (int i = 0; i < w.size(); ++ i) w[i] = v[i] / f;
    return w;
}

ostream& operator <<(ostream &o, const Poly &v) {
    o << "[";
    for (int i = 0; i < v.size(); ++ i) cerr << v[i] << (i != v.size() - 1 ? "," : "]");
    return o;
}

double* Poly::coefficient() {
    return &(d.front());
}

Poly Poly::stripe() const {
    int idx = d.size();
    while (idx && abs(d[idx - 1]) < EPS) idx --;
    Poly w = Poly(idx);
    for (int i = 0; i < w.size(); ++ i) w[i] = d[i];
    return w;
}

#endif
#endif //ARCRENDER_INCLUDE_VECMATH_POLY_HPP
