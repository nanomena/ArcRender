#ifndef sence_hpp
#define sence_hpp

#include <utility>

#include "utils.hpp"
#include "object.hpp"
#include "graph.hpp"
#include "material.hpp"

class Scene
{
    shared_ptr<Object> skybox;
    vector<shared_ptr<KaDanTree>> graphs;

public:
    shared_ptr<Material> env;
    explicit Scene(shared_ptr<Object> skybox_);

    void add_object(const shared_ptr<Object> &object);
    void add_objects(const vector<shared_ptr<Object>> &object);

    void inter(const Ray &ray, shared_ptr<Object> &object, Vec3 &hitpoint) const;
};

#ifdef ARC_IMPLEMENTATION

Scene::Scene(shared_ptr<Object> skybox_)
{
    skybox = std::move(skybox_);
}

void Scene::add_object(const shared_ptr<Object> &object)
{
    vector<shared_ptr<Object>> objects{object};
    graphs.push_back(make_shared<KaDanTree>(objects));
}
void Scene::add_objects(const vector<shared_ptr<Object>> &objects)
{
    graphs.push_back(make_shared<KaDanTree>(objects));
}

void Scene::inter(const Ray &ray, shared_ptr<Object> &object, Vec3 &hitpoint) const
{
    object = skybox;
    int hit;
    skybox->inter(ray, hit, hitpoint);
    assert(hit);
    for (const auto &graph : graphs)
        graph->query(ray, object, hitpoint);
}

#endif
#endif /* sence_hpp */
