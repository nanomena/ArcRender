#ifndef objloader_hpp
#define objloader_hpp

#include "utils.hpp"
#include "vecmath.hpp"
#include "shape.hpp"
#include "bxdf.hpp"
#include "scene.hpp"

#include "bxdfs/mtl.hpp"
#include "shapes/triangle.hpp"
#include "lights/uniform.hpp"

bool LoadModel(const string &filename, const string &mtlDir, const Trans3 &T, const shared_ptr<Medium> &medium, vector<shared_ptr<Shape>> &objects);

#ifdef ARC_IMPLEMENTATION

bool LoadModel(const string &filename, const string &mtlDir, const Trans3 &T, const shared_ptr<Medium> &medium, vector<shared_ptr<Shape>> &objects) {
    cerr << filename << ":" << endl;
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> mtlShapes;
    vector<tinyobj::material_t> mtlMaterials;

    string warn, err;
    bool ret = tinyobj::LoadObj(&attrib, &mtlShapes, &mtlMaterials, &warn, &err, filename.c_str(), mtlDir.c_str());
    if (!warn.empty()) std::cerr << warn << std::endl;
    if (!err.empty()) { std::cout << err << std::endl; return false; }
    if (!ret) { std::cout << ret << std::endl; return false; }
    cerr << "shapes : " << mtlShapes.size() << endl;

    vector<shared_ptr<BxDF>> BxDFs;

    for (const auto &material: mtlMaterials) {
        Spectrum diffuse = rgb(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        Spectrum specular = rgb(material.specular[0], material.specular[1], material.specular[2]);
//        Spectrum emission = rgb(material.emission[0], material.emission[1], material.emission[2]);
        double ior = material.ior, dissolve = material.dissolve, rough = material.roughness;

        BxDFs.push_back(make_shared<MtlGGX>(diffuse, specular, rough, ior, dissolve));
    }

    for (const auto &s: mtlShapes) {
        int index_offset = 0;
        for (int f = 0; f < s.mesh.num_face_vertices.size(); ++f) {
            int fv = s.mesh.num_face_vertices[f];
            assert(fv == 3);
            vector<Vec3> vertices, normals;
            vector<Vec2> textures;
            for (int v = 0; v < fv; ++v) {
                tinyobj::index_t idx = s.mesh.indices[index_offset + v];
                vertices.emplace_back(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );
//                normals.emplace_back(
//                    attrib.normals[3 * idx.vertex_index + 0],
//                    attrib.normals[3 * idx.vertex_index + 1],
//                    attrib.normals[3 * idx.vertex_index + 2]
//                );
//                textures.emplace_back(
//                    attrib.texcoords[2 * idx.texcoord_index + 0],
//                    attrib.texcoords[2 * idx.texcoord_index + 1]
//                );
            }
            index_offset += fv;

            objects.push_back(make_shared<Triangle>(
                BxDFs[s.mesh.material_ids[f]], nullptr,
                medium, medium,
                T * vertices[0], T * vertices[1], T * vertices[2]
            ));
        }
    }
    cerr << "Read Done" << endl;
    return true;
}

#endif
#endif /* objloader_hpp */
