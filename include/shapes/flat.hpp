#ifndef shapes_flat_hpp
#define shapes_flat_hpp

#include "../shape.hpp"

class Flat : public Shape {
public:
    explicit Flat(
        const shared_ptr<BxDF> &BxDF, const shared_ptr<Light> &Light,
        const shared_ptr<Medium> &inside, const shared_ptr<Medium> &outside,
        const vector<Vec3> &vertices
    );

    template<typename... Args>
    explicit Flat(
        const shared_ptr<BxDF> &BxDF, const shared_ptr<Light> &Light,
        const shared_ptr<Medium> &inside, const shared_ptr<Medium> &outside,
        Args... vertices
    ) : Shape(BxDF, Light, inside, outside) {
        vector<Vec3> tmp;
        n = 0;
        (tmp.push_back(vertices), ...);
        new(this)Flat(BxDF, Light, inside, outside, tmp);
    }

    bool intersect(const Ray &ray, double &t) const override;
    Vec3 normal(const Vec3 &inter) const override;
    void sampleSurface(Vec3 &pos, double &pdf, Sampler &RNG) const override;
    double evaluateSurfaceImportance(const Vec3 &pos) const override;

private:
    unsigned int n;
    vector<Vec3> vertices, tVert;
    Vec3 *tVertices;
    Vec3 norm;
    Mat3 T;

    unsigned int onRight(const Vec3 &p1, const Vec3 &p2, const Vec3 &q) const;
};

#ifdef ARC_IMPLEMENTATION

Flat::Flat(
    const shared_ptr<BxDF> &BxDF, const shared_ptr<Light> &Light,
    const shared_ptr<Medium> &inside, const shared_ptr<Medium> &outside,
    const vector<Vec3> &vertices
) : Shape(BxDF, Light, inside, outside), vertices(vertices) {
//    cerr << "S" << endl;
    n = vertices.size();

    for (auto v: vertices) cerr << v << endl;
    norm = ((vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0])).norm();
    Mat3 TInv;
//    cerr << (vertices[1] - vertices[0]).norm() << endl;
//    cerr << (vertices[2] - vertices[0]).norm() << endl;
    rotateAxis(norm, (vertices[1] - vertices[0]).norm(), T, TInv);

//    cerr << "E" << endl;

    tVert.resize(n);
    for (int i = 0; i < n; ++i)
        tVert[i] = T * vertices[i];
    tVertices = &(tVert.front());
    for (int i = 0; i < n; ++i)
        box = box + Box3(vertices[i]);
}

unsigned int Flat::onRight(const Vec3 &p1, const Vec3 &p2, const Vec3 &q) const {
    if (p1.x() > p2.x()) return onRight(p2, p1, q);
    if (q.x() < p1.x()) return 0;
    if (p2.x() <= q.x()) return 0;
    return (q.y() - p1.y()) * (p2.x() - p1.x())
        > (q.x() - p1.x()) * (p2.y() - p1.y());
}

bool Flat::intersect(const Ray &ray, double &t) const {
//    cerr << "here!" << endl;

    Ray TRay = Ray(T * ray.o, T * ray.d);
    if (abs(TRay.d.z()) < EPS) return false;

//    for (int i = 0; i < n; ++i) {
//        cerr << tVertices[i] << " ";
//    }
//    cerr << endl;
//    cerr << T << endl;
    t = (tVertices[0].z() - TRay.o.z()) / TRay.d.z();
    if (t < EPS) return false;
    Vec3 TCandi = TRay.o + TRay.d * t;

    unsigned int inside = onRight(tVertices[0], tVertices[n - 1], TCandi);
    for (int i = 0; i + 1 < n; ++i)
        inside ^= onRight(tVertices[i], tVertices[i + 1], TCandi);

//    cerr << "here!" << " " << ray.o << " " << ray.d << " " << TCandi <<  " " << inside << endl;
    return inside;
}
Vec3 Flat::normal(const Vec3 &inter) const {
    return norm;
}
void Flat::sampleSurface(Vec3 &pos, double &pdf, Sampler &RNG) const { // @TODO better locate method
    double area = 0;
    for (int i = 2; i < n; ++i)
        area += ((vertices[i - 1] - vertices[0]) ^ (vertices[i] - vertices[0])).length() / 2;
    double p = RNG.rand();
    int k = -1;
    for (int i = 2; i < n; ++i) {
        double w = ((vertices[i - 1] - vertices[0]) ^ (vertices[i] - vertices[0])).length() / 2;
        if (p < w / area) {
            k = i;
            break;
        } else p -= w / area;
    }
    double a = RNG.rand(), b = RNG.rand();
    if (a + b > 1) a = 1 - a, b = 1 - b;
    pos = (vertices[k - 1] - vertices[0]) * a + (vertices[k] - vertices[0]) * b + vertices[0];
    pdf = 1 / area;
}
double Flat::evaluateSurfaceImportance(const Vec3 &pos) const {
    double area = 0;
    for (int i = 2; i < n; ++i)
        area += ((vertices[i - 1] - vertices[0]) ^ (vertices[i] - vertices[0])).length() / 2;
    return 1 / area;
}
#endif
#endif /* shapes_flat_hpp */