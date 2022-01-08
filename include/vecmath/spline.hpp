#ifndef ARC_INCLUDE_VECMATH_SPLINE_HPP
#define ARC_INCLUDE_VECMATH_SPLINE_HPP

struct Spline {
    double tdx(int idx) const {
        return idx / (double)(n + k + 1);
    }

    template<typename... Args>
    explicit Spline(int k, Args... point) : k(k) {
        vector<Vec2> P;
        (P.push_back(point), ...);
        n = P.size() - 1;
        map<tuple<int, int, int>, Poly> BMap;
        for (int i = 0; i <= n + k; ++ i) {
            BMap[make_tuple(i, 0, i)] = GenPoly(1);
            for (int j = 1; j <= k; ++ j) {
                for (int ii = i - j; ii <= i; ++ ii) {
                    BMap[make_tuple(i, j, ii)] = BMap[make_tuple(i, j - 1, ii)] * (GenPoly(0, 1) - GenPoly(tdx(ii))) / (tdx(ii + j) - tdx(ii))
                        + BMap[make_tuple(i, j - 1, ii + 1)] * (GenPoly(tdx(ii + j + 1)) - GenPoly(0, 1)) / (tdx(ii + j + 1) - tdx(ii + 1));
                }
            }
        }
        Bx.resize(n + 1);
        By.resize(n + 1);
        dBx.resize(n + 1);
        dBy.resize(n + 1);
        for (int i = k; i <= n; ++ i) {
            for (int ii = i - k; ii <= i; ++ ii) {
                Bx[i] += P[ii].x() * BMap[make_tuple(i, k, ii)];
                By[i] += P[ii].y() * BMap[make_tuple(i, k, ii)];
                dBx[i] += P[ii].x() * ((BMap[make_tuple(i, k - 1, ii)] / (tdx(ii + k) - tdx(ii))
                    - BMap[make_tuple(i, k - 1, ii + 1)] / (tdx(ii + k + 1) - tdx(ii + 1))) * k);
                dBy[i] += P[ii].y() * ((BMap[make_tuple(i, k - 1, ii)] / (tdx(ii + k) - tdx(ii))
                    - BMap[make_tuple(i, k - 1, ii + 1)] / (tdx(ii + k + 1) - tdx(ii + 1))) * k);
            }
            cerr << i << " " << By[i] << " " << dBy[i] << endl;
        }
    }

    Vec2 B(double u) const {
        auto idx = int(u * (n + k + 1));
        if (idx < k) idx = k; if (idx > n) idx = n;
//#pragma omp critical
//        cerr << u << " " << Bx[idx](u) << " " << By[idx](u) << endl;
        return {Bx[idx](u), By[idx](u)};
    }

    Vec2 dB(double u) const {
        auto idx = int(u * (n + k + 1));
        if (idx < k) idx = k; if (idx > n) idx = n;
        return {dBx[idx](u), dBy[idx](u)};
    }

    int n, k;
    vector<Poly> Bx, dBx, By, dBy;
};

#endif //ARC_INCLUDE_VECMATH_SPLINE_HPP
