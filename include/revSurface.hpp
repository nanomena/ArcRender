#ifndef revSurface_hpp
#define revSurface_hpp

#include "utils.hpp"
#include "vecmath.hpp"
#include "shapes/conical.hpp"

class RevSurface {
public:
    RevSurface (
        const BxDF *bxdf, const Light *light,
        const Medium *inside, const Medium *outside,
        const Ray &v, Curve *curve, int resolution
    );
    ~RevSurface();

    vector<const Shape *> get() const;

private:
    vector<const Shape *> objects;
};

#ifdef ARC_IMPLEMENTATION

RevSurface::~RevSurface() {
    for (auto &object : objects)
        delete object;
}

RevSurface::RevSurface (
        const BxDF *bxdf, const Light *light,
        const Medium *inside, const Medium *outside,
        const Ray &v, Curve *curve, int resolution
) {
    vector<conicalNode> faces;
    curve->discretize(resolution, faces);
    for (auto face : faces) {
        objects.push_back(new Conical(
            bxdf, light, inside, outside,
            v, face.v0, face.v1, face.vn0, face.vn1
        ));
    }
}

vector<const Shape *> RevSurface::get() const {
    return objects;
}

#endif
#endif /* revSurface_hpp */
