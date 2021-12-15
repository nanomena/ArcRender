#ifndef photon_hpp
#define photon_hpp

struct Photon {
    Vec3 intersect;
    Ray lB;
    shared_ptr<Shape> object;
    Spectrum color;
};

#endif //photon_hpp
