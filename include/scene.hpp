#ifndef scene_hpp
#define scene_hpp

#include "utils.hpp"
#include "graph.hpp"
#include "shapes/sphere.hpp"

class Scene {
public:
    const Shape *skybox;
    const Camera *camera;
    const Medium *medium; // global medium
    Spectrum skyboxColor;
    vector<const Shape*> lights;

    Scene(const Camera *camera, const Spectrum &skyboxColor, const Medium *medium);
    ~Scene();


    void addObject(const Shape *object, const string &name = "");
    void addObjects(const vector<const Shape *> &object, const string &name = "");
    void clear();

    void intersect(const Ray &ray, const Shape *&object, double &t) const;
    const Medium *visible(const Ray &ray, const Object *object, double t) const;

    Box3 box() const;
private:
    vector<KaDanTree *> graphs;
};

#ifdef ARC_IMPLEMENTATION

Scene::Scene(const Camera *camera, const Spectrum &skyboxColor, const Medium *medium)
    : camera(camera), skyboxColor(skyboxColor), medium(medium) {
    skybox = new Sphere(
        nullptr, nullptr,
        nullptr, medium,
        Vec3(0, 0, 0), INF / 10, true
    );
}
Scene::~Scene() {
    clear();
    delete skybox;
}

void Scene::addObject(const Shape *object, const string &name) {
    vector<const Shape *> objects{object};
    addObjects(objects, name);
}
void Scene::addObjects(const vector<const Shape *> &objects, const string &name) {
    for (const auto &object: objects) {
        if (object->isLight())
            lights.push_back(object);
    }
    graphs.push_back(new KaDanTree(objects));
}
void Scene::clear() {
    for (auto &object: graphs)
        delete object;
    graphs.clear();
}

void Scene::intersect(const Ray &ray, const Shape *&object, double &t) const {
//    cerr << "intersect checking: " << ray.o << " " << ray.d << endl;

    Ray rayX = ray;
    rayX.d = rayX.d.norm();

    object = skybox;
    bool f = skybox->intersect(rayX, t);
    assert(f);
    for (const auto &graph: graphs)
        graph->intersect(rayX, object, t);
}

Box3 Scene::box() const {
    Box3 b;
    for (const auto &graph: graphs)
        b = b + graph->box();
    return b;
}

const Medium *Scene::visible(const Ray &ray, const Object *object, double t) const {
    double tTrue;
    const Shape *objectTrue;
    intersect(ray, objectTrue, tTrue);
    if (tTrue > t + EPS) return objectTrue->getMedium({ray.o + ray.d * tTrue, -ray.d});
    if ((objectTrue != object) || tTrue < t - EPS) return nullptr;
    return objectTrue->getMedium({ray.o + ray.d * tTrue, -ray.d});
}


#endif
#endif /* scene_hpp */
