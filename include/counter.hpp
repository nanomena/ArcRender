#ifndef counter_hpp
#define counter_hpp

#include "spectrum.hpp"

class Counter {
public:
    explicit Counter(int pass = 1);
    explicit Counter(const vector<double> &weight);

    void add(Spectrum color, int idx = 0);
    Spectrum color() const;

    friend ostream &operator <<(ostream &o, const Counter &t);
private:
    int pass;
    vector<double> sum0, weight;
    vector<Spectrum> sum1;
};

#ifdef ARC_IMPLEMENTATION

Counter::Counter(int pass) : pass(pass) {
    sum0.resize(pass);
    sum1.resize(pass);
    weight.resize(pass);
    for (int i = 0; i < pass; ++i) weight[i] = 1.;
}

Counter::Counter(const vector<double> &weight) : weight(weight) {
    pass = weight.size();
    sum0.resize(pass);
    sum1.resize(pass);
}

void Counter::add(Spectrum color, int idx) {
    sum0[idx] = sum0[idx] + 1.;
    sum1[idx] = sum1[idx] + color;
}

Spectrum Counter::color() const {
    double tot0;
    Spectrum tot1;
    for (int i = 0; i < pass; ++i) {
        tot0 = tot0 + sum0[i] * weight[i];
        tot1 = tot1 + sum1[i] * weight[i];
    }
    if (tot0 > EPS) return tot1 / tot0;
    else return Spectrum(0);
}

ostream &operator <<(ostream &o, const Counter &t) {
    for (int i = 0; i < t.pass; ++i) {
        if (t.sum0[i] > EPS)
            o << "(" << t.sum1[i] / t.sum0[i] << ":" << t.sum0[i] << ")";
        else
            o << "(N/A)";
    }
    return o;
}

#endif
#endif //counter_hpp
