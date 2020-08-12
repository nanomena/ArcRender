#ifndef sence_hpp
#define sence_hpp

#include <utility>

#include "utils.hpp"
#include "object.hpp"
#include "graph.hpp"
#include "material.hpp"

class Sence
{
    shared_ptr<Object> skybox;
    vector<shared_ptr<KaDanTree>> graphs;

public:
    shared_ptr<Material> env;
    Sence(shared_ptr<Material> env_, shared_ptr<Object> skybox_);

    void add_object(const shared_ptr<Object> &object);
    void add_objects(const vector<shared_ptr<Object>> &object);

//    shared_ptr<Material> outside();
    double forward(Photon &photon, hit_type &type) const;
};

#ifdef ARC_IMPLEMENTATION

Sence::Sence(shared_ptr<Material> env_, shared_ptr<Object> skybox_)
{
    env = std::move(env_);
    skybox = std::move(skybox_);
}

void Sence::add_object(const shared_ptr<Object> &object)
{
    vector<shared_ptr<Object>> objects{object};
    graphs.push_back(make_shared<KaDanTree>(objects));
}
void Sence::add_objects(const vector<shared_ptr<Object>> &objects)
{
    graphs.push_back(make_shared<KaDanTree>(objects));
}

double Sence::forward(Photon &photon, hit_type &type) const
{
    shared_ptr<Object> next = skybox;
    int hit;
    Vec3 hitpoint;
    skybox->inter(photon.ray, hit, hitpoint);
    for (const auto &graph : graphs)
        graph->query(photon.ray, next, hitpoint);

    photon.trans(photon.inside->through((hitpoint - photon.ray.o).norm()));
    double weight = next->forward(hitpoint, photon, type);
    return weight;
}

#endif
#endif /* sence_hpp */
