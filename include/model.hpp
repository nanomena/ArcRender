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

bool LoadModel(const string &filename, const string &mtlDir, const Trans3 &T, const shared_ptr<Medium> &medium, const shared_ptr<Scene> &scene);

struct mtlMaterial {
    Spectrum diffuse, specular;
    shared_ptr<Texture> diffuseTex, specularTex;
    double roughness{}, ior{}, dissolve{};
};

#ifdef ARC_IMPLEMENTATION

bool LoadModel(const string &filename, const string &mtlDir, const Trans3 &T, const shared_ptr<Medium> &medium, const shared_ptr<Scene> &scene) {
    cerr << filename << ":" << endl;
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> mtlShapes;
    vector<tinyobj::material_t> mtlMaterials;

    string warn, err;
    bool ret = tinyobj::LoadObj(&attrib, &mtlShapes, &mtlMaterials, &warn, &err, filename.c_str(), mtlDir.c_str());
    if (!warn.empty()) std::cerr << warn << std::endl;
    if (!err.empty()) { std::cout << "Error: " << err << std::endl; return false; }
    if (!ret) { std::cout << ret << std::endl; return false; }
    cerr << "shapes : " << mtlShapes.size() << endl;

    vector<mtlMaterial> mtlList;

    for (const auto &material: mtlMaterials) {
        Spectrum diffuse = rgb(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        Spectrum specular = rgb(material.specular[0], material.specular[1], material.specular[2]);
        double ior = material.ior, dissolve = material.dissolve, roughness = 1 - atan(material.shininess);
        shared_ptr<Texture> diffuseTex, specularTex;

        if (!material.diffuse_texname.empty()) {
            string texture = mtlDir + material.diffuse_texname;
            cerr << "Diffuse Texture: " << texture << endl;
            diffuseTex = make_shared<Texture>(texture.c_str());
        }
        if (!material.specular_texname.empty()) {
            string texture = mtlDir + material.specular_texname;
            cerr << "Specular Texture: " << texture << endl;
            specularTex = make_shared<Texture>(texture.c_str());
        }

        mtlList.push_back({
            diffuse, specular, diffuseTex, specularTex, roughness, ior, dissolve
        });
    }

    for (const auto &s: mtlShapes) {
        if (attrib.normals.empty()) {
            cerr << "Missing normal!" << endl;
        }


        vector<shared_ptr<Shape>> objects;
        int index_offset = 0;
        for (int f = 0; f < s.mesh.num_face_vertices.size(); ++f) {
            int fv = s.mesh.num_face_vertices[f];
            assert(fv == 3);
            vector<Vec3> vertices, normals, textures;
            for (int v = 0; v < fv; ++v) {
                tinyobj::index_t idx = s.mesh.indices[index_offset + v];
                vertices.emplace_back(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );
                if (!attrib.normals.empty())
                    normals.emplace_back(
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    );
                textures.emplace_back(
                    attrib.texcoords[2 * idx.texcoord_index + 0],
                    attrib.texcoords[2 * idx.texcoord_index + 1],
                    0
                );
            }
            if (normals.empty()) {
                Vec3 normal = ((vertices[1] - vertices[0]) ^ (vertices[2] - vertices[0])).norm();
                for (int v = 0; v < fv; ++v) normals.push_back(normal);
            }
            index_offset += fv;

            auto &material = mtlList[s.mesh.material_ids[f]];
            Trans3 texT = Trans3(
                T * vertices[0], T * vertices[1], T * vertices[2],
                textures[0], textures[1], textures[2]
                );
            TextureMap diffuse = (material.diffuseTex != nullptr)
                ? TextureMap(material.diffuseTex, texT)
                : TextureMap(material.diffuse),
                specular = (material.specularTex != nullptr)
                ? TextureMap(material.specularTex, texT)
                : TextureMap(material.specular);

            objects.push_back(make_shared<Triangle>(
                make_shared<MtlGGX>(diffuse, specular, material.roughness, material.ior, material.dissolve),
                nullptr,
                medium, medium,
                T * vertices[0], T * vertices[1], T * vertices[2],
                (T.t * normals[0]).norm(), (T.t * normals[1]).norm(), (T.t * normals[2]).norm()
            ));
        }
        scene->addObjects(objects);
    }
    cerr << "Read Done" << endl;
    return true;
}

#endif
#endif /* objloader_hpp */
