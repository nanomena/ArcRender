#ifndef ARC_INCLUDE_VECMATH_UTILS_HPP
#define ARC_INCLUDE_VECMATH_UTILS_HPP

extern const double EPS;
extern const double INF;
extern const double pi;

#ifdef ARC_IMPLEMENTATION

const double EPS = 1e-9;
const double INF = 1e50;
const double pi = acos(-1);

#endif
#endif //ARC_INCLUDE_VECMATH_UTILS_HPP
