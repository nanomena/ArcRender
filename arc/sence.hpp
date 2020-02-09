#ifndef SENCE
#define SENCE
#include "arc.hpp"
#include "object.hpp"
#include "graph.hpp"

class Sence
{
    shared_ptr<Object> skybox;
    vector<shared_ptr<Object>> objects;
    KaDanTree graph;

public:
    Sence (shared_ptr<Object> _skybox)
    {
        skybox = _skybox;
    }

    void add_object(shared_ptr<Object> object)
    {
        objects.push_back(object);
    }
    void build_graph()
    {
        graph.load(&(objects.front()), objects.size());
    }

    double forward(Photon &photon, int &type) const
    {
        shared_ptr<Object> next = skybox;
        int hit; Vec3 hitpoint;
        skybox->inter(photon.ray, hit, hitpoint);
        graph.query(photon.ray, next, hitpoint);

        photon.trans(photon.inside->through((hitpoint - photon.ray.o).norm()));
        double weight = next->forward(hitpoint, photon, type);
        return weight;
    }
};

#endif
