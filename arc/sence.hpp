#ifndef SENCE
#define SENCE
#include "object.hpp"
#include "camera.hpp"
#include <memory>

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
    Photon forward(Photon photon) const
    {
        shared_ptr<Object> next = objects[0];
        Vec3 hitpoint(INF, INF, INF);
        for (shared_ptr<Object> object : objects)
        {
            Vec3 p = object->inter(photon);
            if ((p - photon.ray.o).norm2() < (hitpoint - photon.ray.o).norm2())
            {
                hitpoint = p;
                next = object;
            }
        }
        return next->forward(photon, hitpoint);
    }
};

#endif
