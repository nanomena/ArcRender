#ifndef ARC_INCLUDE_VECMATH_CURVE_HPP
#define ARC_INCLUDE_VECMATH_CURVE_HPP

struct Curve {
public:
    template<typename... Args>
    explicit Curve(int k, Args... point) : k(k) {
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
        for (int i = k; i <= n; ++ i) {
            L[i] = tdx(i), U[i] = tdx(i + 1);
            for (int ii = i - k; ii <= i; ++ ii) {
                B[i] += P[ii] * BMap[make_tuple(i, k, ii)];
                dB[i] += P[ii] * ((BMap[make_tuple(i, k - 1, ii)] / (tdx(ii + k) - tdx(ii))
                    - BMap[make_tuple(i, k - 1, ii + 1)] / (tdx(ii + k + 1) - tdx(ii + 1))) * k);
            }
        }
    }

    int n, k;
    vector<double> P, L, U;
    vector<Poly> B, dB;

    double tdx(int idx) const {
        return idx / (double)(n + k + 1);
    }
};

#endif //ARC_INCLUDE_VECMATH_CURVE_HPP
