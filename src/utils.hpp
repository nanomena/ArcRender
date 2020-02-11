#ifndef ARC
#define ARC

// #pragma GCC optimize("inline","Ofast")
#pragma GCC target("sse3","sse2","sse")
#pragma GCC target("avx","sse4","sse4.1","sse4.2","ssse3")
#pragma GCC target("f16c")
#pragma GCC optimize("fast-math","unroll-loops","no-stack-protector")


#ifdef ebug
    #define DEBUG 1
#else
    #define DEBUG 0
#endif

#ifdef ARC_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION
#endif

#define $ if (DEBUG) cerr
// #define double long double

#include <random>
#include <memory>
#include <vector>
#include <cmath>
#include <iostream>
#include <functional>
#include <string>
#include <fstream>
#include <stdarg.h>
#include <algorithm>
#include "stb_image.h"
using namespace std;



#endif
