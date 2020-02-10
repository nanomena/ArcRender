#ifndef sence_hpp
#define sence_hpp

#include "utils.hpp"
#include "object.hpp"
#include "graph.hpp"
#include "material.hpp"

class Sence
{
    shared_ptr<Object> skybox;
    vector<shared_ptr<Object>> objects;
    KaDanTree graph;

public:
    shared_ptr<Material> env;
    Sence (shared_ptr<Material> _env, shared_ptr<Object> _skybox);

    void add_object(shared_ptr<Object> object);
    void build_graph();

    shared_ptr<Material> outside();
    double forward(Photon &photon, int &type) const;
};

#ifndef library

Sence::Sence (shared_ptr<Material> _env, shared_ptr<Object> _skybox)
{
    env = _env;
    skybox = _skybox;
}

void Sence::add_object(shared_ptr<Object> object)
{
    objects.push_back(object);
}
void Sence::build_graph()
{
    graph.load(&(objects.front()), static_cast<int>(objects.size()));
}

double Sence::forward(Photon &photon, int &type) const
{
    shared_ptr<Object> next = skybox;
    int hit; Vec3 hitpoint;
    skybox->inter(photon.ray, hit, hitpoint);
    graph.query(photon.ray, next, hitpoint);

    photon.trans(photon.inside->through((hitpoint - photon.ray.o).norm()));
    double weight = next->forward(hitpoint, photon, type);
    return weight;
}

#endif
#endif /* sence_hpp */
