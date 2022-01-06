#ifndef ARC_INCLUDE_VECMATH_CURVE_HPP
#define ARC_INCLUDE_VECMATH_CURVE_HPP

struct conicalNode {
    Vec2 v0, v1, vn0, vn1;
};

//class Curve {
//public:
//    template<typename... Args>
//    Curve(int k, Args... point) : k(k) {
//        (points.push_back(point), ...);
//        n = points.size() - 1;
//        for (int i = 0; i <= n; ++ i) {
//            BMap[make_tuple(i, 0, i)] = GenPoly(1);
//            for (int j = 1; j <= k; ++ j) {
//                for (int ii = i - j; ii <= i; ++ ii) {
//                    BMap[make_tuple(i, j, ii)] = B(i, j - 1, ii) * (GenPoly(0, 1) - GenPoly(tdx(ii))) / (tdx(ii + j) - tdx(ii))
//                        + B(i, j - 1, ii + 1) * (GenPoly(tdx(ii + j + 1)) - GenPoly(0, 1)) / (tdx(ii + j + 1) - tdx(ii + 1));
//
//                    cerr << "B: " << i << " " << j << " " << ii << " | " << BMap[make_tuple(i, j, ii)] << endl;
//                }
//            }
//        }
//    }
//
//    Poly B(int i, int j, int ii) const;
//    Poly dB(int i, int j, int ii) const;
//
//    void query(int idx, double &l, double &u, Poly &x, Poly &y) const;
//    void dQuery(int idx, double &l, double &u, Poly &x, Poly &y) const;
//
//    int getN() const;
//    int getK() const;
//
//    void discretize(int resolution, vector<conicalNode> &data) const;
//
//private:
//    int n, k;
//    vector<Vec2> points;
//    map<tuple<int, int, int>, Poly> BMap;
//
//    double tdx(int idx) const;
//};


class Curve {
public:
    template<typename... Args>
    explicit Curve(int k, Args... point) : k(k) {
        (points.push_back(point), ...);
    }

    double tdx(int idx) {
        return idx / (double) (points.size() + k);
    }

    double B(int i, int n, double t) {
        if (n == 0) return (double) ((tdx(i) <= t) && (t < tdx(i + 1)));
        if (Bst.count(std::make_tuple(i, n, t)))
            return Bst[std::make_tuple(i, n, t)];
        double b = (t - tdx(i)) / (tdx(i + n) - tdx(i)) * B(i, n - 1, t)
            + (tdx(i + n + 1) - t) / (tdx(i + n + 1) - tdx(i + 1))
                * B(i + 1, n - 1, t);
        return Bst[std::make_tuple(i, n, t)] = b;
    }

    double dB(int i, int n, double t) {
        return n * (
            B(i, n - 1, t) / (tdx(i + n) - tdx(i))
                - B(i + 1, n - 1, t) / (tdx(i + n + 1) - tdx(i + 1)));
    }

    void discretize(int resolution, vector<conicalNode> &data) {
        data.clear();
        for (int p = resolution * k; p < resolution * (points.size()); ++ p) {
            double t0 = 1. * (p + 0) / (double)(resolution * (points.size() + k));
            double t1 = 1. * (p + 1) / (double)(resolution * (points.size() + k));
            Vec2 v0, v1, vt0, vt1;
            for (int j = 0; j < points.size(); ++ j) {
                v0 += points[j] * B(j, k, t0);
                v1 += points[j] * B(j, k, t1);
                vt0 += points[j] * dB(j, k, t0);
                vt1 += points[j] * dB(j, k, t1);
            }
            Vec2 vn0 = Vec2(vt0.y(), -vt0.x()).norm(),
                vn1 = Vec2(vt1.y(), -vt1.x()).norm();
            data.push_back({v0, v1, vn0, vn1});
        }
    }

protected:
    std::map<std::tuple<int, int, double>, double> Bst;
    vector<Vec2> points;
    int k;
};

#ifdef ARC_IMPLEMENTATION


//double Curve::tdx(int idx) const {
//    return idx / (double)(n + k + 1);
//}
//
//Poly Curve::B(int i, int j, int ii) const {
//    return BMap.count(make_tuple(i, j, ii)) ? BMap.at(make_tuple(i, j, ii)) : Poly(0);
//}
//
//Poly Curve::dB(int i, int j, int ii) const {
//    return (B(i, j - 1, ii) / (tdx(ii + j) - tdx(ii))
//        - B(i, j - 1, ii + 1) / (tdx(ii + j + 1) - tdx(ii + 1))) * j;
//}
//
//void Curve::query(int idx, double &l, double &u, Poly &x, Poly &y) const {
//    x = Poly(), y = Poly();
//    l = tdx(idx), u = tdx(idx + 1);
//    for (int i = 0; i <= k; ++ i) {
//        x += points[idx - k + i].x() * B(idx - k + i, k, idx);
//        y += points[idx - k + i].y() * B(idx - k + i, k, idx);
//    }
//    cerr << "Test Begin: " << endl;
//    for (int i = 0; i <= k; ++ i) cerr << B(idx - k + i, k, idx)(l) << " ";
//    cerr << endl;
//    cerr << "Test End: " << endl;
//}
//
//void Curve::dQuery(int idx, double &l, double &u, Poly &x, Poly &y) const {
//    x = Poly(), y = Poly();
//    l = tdx(idx), u = tdx(idx + 1);
//    for (int i = 0; i <= k; ++ i) {
//        x += points[idx - k + i].x() * dB(idx - k + i, k, idx);
//        y += points[idx - k + i].y() * dB(idx - k + i, k, idx);
//    }
//}
//
//int Curve::getN() const { return n; }
//int Curve::getK() const { return k; }
//
//void Curve::discretize(int resolution, vector<conicalNode> &data) const {
//    for (int i = k; i <= n; ++ i) {
//        Poly x, y, dx, dy;
//        double l, u;
//        query(i, l, u, x, y);
//        dQuery(i, l, u, dx, dy);
//        cerr << l << " " << u << endl;
//        for (int p = 0; p < resolution; ++ p) {
//            double t0 = l + (u - l) / resolution * p, t1 = t0 + (u - l) / resolution;
//            data.push_back({
//                {x(t0),  y(t0)},
//                {x(t1),  y(t1)},
//                Vec2(-dy(t0), dx(t0)).norm(),
//                Vec2(-dy(t1), dx(t1)).norm()
//            } );
//        }
//    }
//}

#endif
#endif //ARC_INCLUDE_VECMATH_CURVE_HPP
