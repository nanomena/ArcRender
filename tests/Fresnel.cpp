#include <bits/stdc++.h>
#define DEBUG 0
// #define double long double
#define $ if (DEBUG) cerr
#include "arc/bxdf.hpp"
using namespace std;

double IOR;
double angle, sini, cosi, sint, cost;
int main()
{
    cin >> IOR;
    Trans t(IOR, 0);
    cin >> angle;
    sini = sin(angle);
    cosi = sqrt(1 - sini * sini);
    sint = sini * IOR;
    cost = sqrt(1 - sint * sint);
    cout << t.F(cosi, cost) << endl;
}
