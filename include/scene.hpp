#ifndef scene_hpp
#define scene_hpp

#include "utils.hpp"
#include "graph.hpp"
#include "material.hpp"

class Scene {
public:
    shared_ptr<Shape> skybox;
    shared_ptr<Camera> camera;
    vector<shared_ptr<Shape>> lights;

    Scene(const shared_ptr<Camera> &camera, const shared_ptr<Shape> &skybox);

    void addObject(const shared_ptr<Shape> &object, const string &name = "");
    void addObjects(const vector<shared_ptr<Shape>> &object, const string &name = "");

    void intersect(const Ray &ray, shared_ptr<Shape> &object, double &t) const;
    bool visible(const Ray &test, const shared_ptr<Shape> &object, double dis) const;

private:
    vector<shared_ptr<KaDanTree>> graphs;
};

#ifdef ARC_IMPLEMENTATION

Scene::Scene(const shared_ptr<Camera> &camera, const shared_ptr<Shape> &skybox) : camera(camera), skybox(skybox) {}

void Scene::addObject(const shared_ptr<Shape> &object, const string &name) {
    vector<shared_ptr<Shape>> objects{object};
    addObjects(objects, name);
}
void Scene::addObjects(const vector<shared_ptr<Shape>> &objects, const string &name) {
    for (const auto &object: objects) {
        object->setIdentifier(name);
        if (object->isLight())
            lights.push_back(object);
    }
    graphs.push_back(make_shared<KaDanTree>(objects));
}

void Scene::intersect(const Ray &ray, shared_ptr<Shape> &object, double &t) const {
//    cerr << "intersect checking: " << ray.o << " " << ray.d << endl;

    Ray rayX = ray;
    rayX.d = rayX.d.norm();

    object = skybox;
    bool f = skybox->intersect(rayX, t);
    assert(f);
    for (const auto &graph: graphs)
        graph->intersect(rayX, object, t);
}

bool Scene::visible(const Ray &test, const shared_ptr<Shape> &object, double dis) const {
    double t;
    shared_ptr<Shape> actual;
    intersect(test, actual, t);
    return actual == object && (abs(t - dis) < EPS);
}

#endif
#endif /* scene_hpp */
