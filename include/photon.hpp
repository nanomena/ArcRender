#ifndef photon_hpp
#define photon_hpp

struct Photon {
    Vec3 intersect;
    Ray lB;
    shared_ptr<Shape> object;
    double lastPdf;
    double pdfSum2;
    Spectrum color;
};

#endif //photon_hpp
