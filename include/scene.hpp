#ifndef scene_hpp
#define scene_hpp

#include "utils.hpp"
#include "object.hpp"
#include "graph.hpp"
#include "material.hpp"

class Scene {
    shared_ptr<Object> skybox;
    vector<shared_ptr<KaDanTree>> graphs;

public:
    vector<shared_ptr<Object>> Lights;

    explicit Scene(shared_ptr<Object> skybox_);

    void add_object(const shared_ptr<Object> &object);
    void add_objects(const vector<shared_ptr<Object>> &object);

    void intersect(const Ray &ray, shared_ptr<Object> &object, double &t) const;
};

#ifdef ARC_IMPLEMENTATION

Scene::Scene(shared_ptr<Object> skybox_) {
    skybox = std::move(skybox_);
}

void Scene::add_object(const shared_ptr<Object> &object) {
    vector<shared_ptr<Object>> objects{object};
    add_objects(objects);
}
void Scene::add_objects(const vector<shared_ptr<Object>> &objects) {
    for (const auto &object: objects)
        if (object->surface_info().second > 0)
            Lights.push_back(object);
    graphs.push_back(make_shared<KaDanTree>(objects));
}

void Scene::intersect(const Ray &ray, shared_ptr<Object> &object, double &t) const {
    Ray rayX = ray;
    rayX.d = rayX.d.norm();

    object = skybox;
    bool f = skybox->intersect(rayX, t);
    assert(f);
    for (const auto &graph: graphs)
        graph->intersect(rayX, object, t);
}

#endif
#endif /* scene_hpp */
