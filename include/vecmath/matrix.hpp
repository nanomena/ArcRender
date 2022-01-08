#ifndef ARC_INCLUDE_VECMATH_MATRIX_HPP
#define ARC_INCLUDE_VECMATH_MATRIX_HPP

class Poly;

class Matrix {
public:
    Matrix() = default;
    Matrix(int w, int h);
    explicit Matrix(const double d[]);

    int W() const;
    int H() const;
    const double &operator ()(int i, int j) const;
    double &operator ()(int i, int j);

    Matrix &operator +=(const Matrix &m);
    Matrix &operator -=(const Matrix &m);
    Matrix &operator *=(const Matrix &m);
    Matrix &operator *=(double f);
    Matrix &operator /=(double f);

    Matrix T() const;

protected:
    int w, h;
    vector<double> d;// Row major!
};

Matrix operator +(const Matrix &m0, const Matrix &m1);
Matrix operator -(const Matrix &m0, const Matrix &m1);
Matrix operator *(const Matrix &m0, const Matrix &m1);
Matrix operator *(double f1, const Matrix &m0);
Matrix operator *(const Matrix &m0, double f1);
Matrix operator /(const Matrix &m0, double f1);

ostream &operator <<(ostream &o, const Matrix &v);

void QRDecompose(const Matrix &A, Matrix &Q, Matrix &R);
int QRPivot(Matrix &A, double err, double itrLimit);
vector<double> Root(const Poly &A);

#ifdef ARC_IMPLEMENTATION

Matrix::Matrix(int w, int h) : w(w), h(h) {
    d.resize(w * h);
}

int Matrix::W() const { return w; }
int Matrix::H() const { return h; }
const double &Matrix::operator ()(int i, int j) const { return d[i * w + j]; }
double &Matrix::operator ()(int i, int j) { return d[i * w + j]; }

Matrix &Matrix::operator +=(const Matrix &m) { return (*this) = (*this) + m; }
Matrix &Matrix::operator -=(const Matrix &m) { return (*this) = (*this) - m; }
Matrix &Matrix::operator *=(const Matrix &m) { return (*this) = (*this) * m; }
Matrix &Matrix::operator *=(double f) { return (*this) = (*this) * f; }
Matrix &Matrix::operator /=(double f) { return (*this) = (*this) / f; }

Matrix operator +(const Matrix &m0, const Matrix &m1) {
    assert(m0.H() == m1.H());
    assert(m0.W() == m1.W());
    Matrix m2(m0.W(), m0.H());
    for (int i = 0; i < m0.W(); ++ i)
        for (int j = 0; j < m0.H(); ++ j)
            m2(i, j) = m0(i, j) + m1(i, j);
    return m2;
}

Matrix operator -(const Matrix &m0, const Matrix &m1) {
    assert(m0.H() == m1.H());
    assert(m0.W() == m1.W());
    Matrix m2(m0.W(), m0.H());
    for (int i = 0; i < m0.W(); ++ i)
        for (int j = 0; j < m0.H(); ++ j)
            m2(i, j) = m0(i, j) - m1(i, j);
    return m2;
}

Matrix operator *(double f1, const Matrix &m0) {
    Matrix m2(m0.W(), m0.H());
    for (int i = 0; i < m0.W(); ++ i)
        for (int j = 0; j < m0.H(); ++ j)
            m2(i, j) = m0(i, j) * f1;
    return m2;
}

Matrix operator *(const Matrix &m0, double f1) {
    Matrix m2(m0.W(), m0.H());
    for (int i = 0; i < m0.W(); ++ i)
        for (int j = 0; j < m0.H(); ++ j)
            m2(i, j) = m0(i, j) * f1;
    return m2;
}

Matrix operator /(const Matrix &m0, double f1) {
    Matrix m2(m0.W(), m0.H());
    for (int i = 0; i < m0.W(); ++ i)
        for (int j = 0; j < m0.H(); ++ j)
            m2(i, j) = m0(i, j) / f1;
    return m2;
}

Matrix operator *(const Matrix &m0, const Matrix &m1) {
    assert(m0.H() == m1.W());
    Matrix m2(m0.W(), m1.H());
    for (int i = 0; i < m0.W(); ++i)
        for (int j = 0; j < m1.H(); ++j)
            for (int k = 0; k < m0.H(); ++k)
                m2(i, j) += m0(i, k) * m1(k, j);
    return m2;
}

Matrix Matrix::T() const {
    Matrix m(H(), W());
    for (int i = 0; i < W(); ++ i)
        for (int j = 0; j < H(); ++ j)
            m(j, i) = (*this).operator ()(i, j);
    return m;
}

ostream &operator <<(ostream &o, const Matrix &v) {
    for (int i = 0; i < v.W(); ++i) {
        o << (i == 0 ? "[[" : " [");
        for (int j = 0; j < v.H(); ++j)
            o << v(i, j) << (j < v.H() - 1 ? "," : "]");
        o << (i == v.W() ? "]" : ",") << endl;
    }
    return o;
}

Matrix eye(int n) {
    Matrix m(n, n);
    for (int i = 0; i < n; ++ i) m(i, i) = 1;
    return m;
}

void QRDecompose(const Matrix &A, Matrix &Q, Matrix &R) {
    int n = A.W(), m = A.H();
    Q = eye(n);
    R = A;
    for (int i = 0; i < m; ++ i) {
        Matrix w(n, 1);
        for (int j = i; j < n; ++ j) w(j, 0) = A(j, i);
        w(i, 0) += sqrt((w.T() * w)(0, 0)) * (w(i, 0) > 0 ? 1 : -1);
        w /= sqrt((w.T() * w)(0, 0));
        Matrix H = eye(n) - 2 * (w * w.T());
        Q = Q * H;
        R = H * A;
    }
}

int QRPivot(Matrix &A, double err, double itrLimit) {
    int n = A.H(), k = n - 1;
    for (int itr = 0; itr < itrLimit && k > 0; ++ itr) {
#pragma omp critical
        cerr << A << endl;
        double s = A(k, k);
        Matrix Q{}, R{};
        QRDecompose(A - eye(n) * s, Q, R);
        A = R * Q + eye(n) * s;
        bool reduce = true;
        for (int i = 0; i < k; ++ i) if (abs(A(k, i)) > err) reduce = false;
        if (reduce) k --;
    }
    return k == 0;
}

vector<double> Root(const Poly &A) {
    Matrix R(A.size(), A.size());
    for (int i = 1; i < A.size(); ++ i) R(i - 1, i) = 1;
    for (int i = 0; i < A.size(); ++ i) R(A.size() - 1, i) = -A[i];
    QRPivot(R, EPS, 200);
    vector<double> v;
    for (int i = 0; i < A.size(); ++ i) {
        double r = R(i, i);
        if (abs(A(r)) < EPS * 1000) v.push_back(r);
    }
    return v;
}

#endif
#endif //ARC_INCLUDE_VECMATH_MATRIX_HPP
