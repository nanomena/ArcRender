#ifndef SENCE
#define SENCE
#include <memory>
#include "object.hpp"

class Sence
{
    vector<shared_ptr<Object>> objects;

public:
    Sence (shared_ptr<Object> skybox)
    {
        objects.push_back(skybox);
    }

    void add_object(shared_ptr<Object> object)
    {
        objects.push_back(object);
    }
    double forward(Photon &photon) const
    {
        shared_ptr<Object> next = objects[0];
        Vec3 hitpoint(INF, INF, INF);
        for (shared_ptr<Object> object : objects)
        {
            Vec3 p = object->inter(photon.ray);
            if ((p - photon.ray.o).norm2() < (hitpoint - photon.ray.o).norm2())
            {
                hitpoint = p;
                next = object;
            }
        }
        double weight = next->forward(hitpoint, photon);
        photon.move(EPS);
        return weight;
    }
};

#endif
