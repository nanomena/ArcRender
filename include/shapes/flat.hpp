#ifndef shapes_flat_hpp
#define shapes_flat_hpp

#include "../shape.hpp"

class Flat : public Shape {
    unsigned int n;
    vector<Vec3> vertices, T_vertices;
    Vec3 norm;
    Mat3 T;

    unsigned int onRight(const Vec3 &p1, const Vec3 &p2, const Vec3 &q) const;

public:
    explicit Flat(
        const shared_ptr<BxDF> &BxDF, const shared_ptr<Light> &Light, const vector<Vec3> &vertices
    );

    template<typename... Args>
    explicit Flat(const shared_ptr<BxDF> &BxDF, const shared_ptr<Light> &Light, Args... vertices) : Shape(BxDF, Light) {
        vector<Vec3> tmp;
        n = 0;
        (tmp.push_back(vertices), ...);
        new(this)Flat(BxDF, Light, tmp);
    }

    bool intersect(const Ray &ray, double &t) const override;
    Vec3 normal(const Vec3 &inter) const override;
    void sample(Vec3 &pos, double &pdf) const override;
};

#ifdef ARC_IMPLEMENTATION

Flat::Flat(
    const shared_ptr<BxDF> &BxDF, const shared_ptr<Light> &Light, const vector<Vec3> &vertices
) : Shape(BxDF, Light), vertices(vertices) {
//    cerr << "S" << endl;
    n = vertices.size();

    for (auto v : vertices) cerr << v << endl;
    norm = ((vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0])).norm();
    Mat3 TInv;
//    cerr << (vertices[1] - vertices[0]).norm() << endl;
//    cerr << (vertices[2] - vertices[0]).norm() << endl;
    rotateAxis(norm, (vertices[1] - vertices[0]).norm(), T, TInv);

//    cerr << "E" << endl;

    T_vertices.resize(n);
    for (int i = 0; i < n; ++i)
        T_vertices[i] = T * vertices[i];
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

    Ray T_ray = Ray(T * ray.o, T * ray.d);
    if (abs(T_ray.d.z()) < EPS) return false;

//    for (int i = 0; i < n; ++i) {
//        cerr << T_vertices[i] << " ";
//    }
//    cerr << endl;
//    cerr << T << endl;
    t = (T_vertices[0].z() - T_ray.o.z()) / T_ray.d.z();
    if (t < EPS) return false;
    Vec3 T_candi = T_ray.o + T_ray.d * t;

    unsigned int inside = onRight(T_vertices[0], T_vertices[n - 1], T_candi);
    for (int i = 0; i + 1 < n; ++i)
        inside ^= onRight(T_vertices[i], T_vertices[i + 1], T_candi);

//    cerr << "here!" << " " << ray.o << " " << ray.d << " " << T_candi <<  " " << inside << endl;
    return inside;
}
Vec3 Flat::normal(const Vec3 &inter) const {
    return norm;
}
void Flat::sample(Vec3 &pos, double &pdf) const { // @TODO better locate method
    double area = 0;
    for (int i = 2; i < n; ++i)
        area += ((vertices[i - 1] - vertices[0]) ^ (vertices[i] - vertices[0])).length() / 2;
    double p = RD.rand();
    int k = -1;
    for (int i = 2; i < n; ++i) {
        double w = ((vertices[i - 1] - vertices[0]) ^ (vertices[i] - vertices[0])).length() / 2;
        if (p < w / area) {
            k = i;
            break;
        } else p -= w / area;
    }
    double a = RD.rand(), b = RD.rand();
    if (a + b > 1) a = 1 - a, b = 1 - b;
    pos = (vertices[k - 1] - vertices[0]) * a + (vertices[k] - vertices[0]) * b + vertices[0];
    pdf = 1 / area;
}

#endif
#endif /* shapes_flat_hpp */