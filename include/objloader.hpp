#ifndef objloader_hpp
#define objloader_hpp

#include "utils.hpp"
#include "vecmath.hpp"
#include "object.hpp"
#include "bxdf.hpp"
#include "scene.hpp"

#include "bxdfs/ggx.hpp"
#include "shapes/triangle.hpp"
#include "lights/uniform.hpp"
// @TODO so this part has sicking coupling

class ObjGroup {
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> mtl_shapes;
    vector<tinyobj::material_t> mtl_materials;
    vector<shared_ptr<Surface>> surfaces;
    vector<shared_ptr<Object>> objects;
    map<string, shared_ptr<Mapping>> mappings;

public:
    Trans3 T;

    void load_materials();
    void load_objects();
    void load(const string &filename);
    void import_to(const shared_ptr<Scene> &scene);
};

#ifdef ARC_IMPLEMENTATION

shared_ptr<Surface> mtl_dealer(const tinyobj::material_t &material) {
    Spect diffuse = rgb(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
    Spect emission = rgb(material.emission[0], material.emission[1], material.emission[2]);
    Spect specular = rgb(material.specular[0], material.specular[1], material.specular[2]);
    double ior = material.ior, dissolve = material.dissolve, rough = material.roughness;
    vector<pair<shared_ptr<BxDF>, Spect>> BxDFs;
    vector<pair<shared_ptr<Light>, Spect>> Lights;

    if (specular.norm() > EPS) BxDFs.emplace_back(make_shared<GGX>(ior, rough), diffuse);
    if (emission.norm() > EPS) Lights.emplace_back(make_shared<UniformLight>(), emission);
    return make_shared<Surface>(BxDFs, Lights);
}

shared_ptr<Object> obj_dealer(const shared_ptr<Shape> &shape, const shared_ptr<Surface> &surface) {
    return make_shared<Object>(shape, surface, "imported");
}

void ObjGroup::load_materials() {
    for (const auto &mtl_material: mtl_materials)
        surfaces.push_back(mtl_dealer(mtl_material));
}

void ObjGroup::load_objects() {
    for (const auto &s: mtl_shapes) {
        int index_offset = 0;
        for (int f = 0; f < s.mesh.num_face_vertices.size(); ++f) {
            int fv = s.mesh.num_face_vertices[f];
            assert(fv == 3);
            vector<Vec3> V, Vn, Vt;
            for (int v = 0; v < fv; ++v) {
                tinyobj::index_t idx = s.mesh.indices[index_offset + v];
                V.emplace_back(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );
//                Vn.emplace_back(
//                    attrib.normals[3 * idx.vertex_index + 0],
//                    attrib.normals[3 * idx.vertex_index + 1],
//                    attrib.normals[3 * idx.vertex_index + 2]
//                );
//                Vt.emplace_back(
//                    attrib.texcoords[2 * idx.texcoord_index + 0],
//                    attrib.texcoords[2 * idx.texcoord_index + 1],
//                    0
//                );
            }
            index_offset += fv;
            shared_ptr<Shape> shape = make_shared<Triangle>(
                T.apply(V[0]), T.apply(V[1]), T.apply(V[2])
            );
            objects.push_back(obj_dealer(shape, surfaces[s.mesh.material_ids[f]]));
        }
    }
}

void ObjGroup::load(const string &filename) {
    string warn, err;
    bool ret = tinyobj::LoadObj(&attrib, &mtl_shapes, &mtl_materials, &warn, &err, filename.c_str());
    if (!warn.empty()) std::cerr << warn << std::endl;
    if (!err.empty()) std::cout << err << std::endl;
    if (!ret) return;
    cerr << "shapes : " << mtl_shapes.size() << endl;
    load_materials();
    load_objects();
}

void ObjGroup::import_to(const shared_ptr<Scene> &scene) {
    scene->add_objects(objects);
}
#endif
#endif /* objloader_hpp */
