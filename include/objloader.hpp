#ifndef objloader_hpp
#define objloader_hpp

#include "utils.hpp"
#include "geometry.hpp"
#include "object.hpp"
#include "bxdf.hpp"
#include "sence.hpp"

class ObjLoader
{
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    vector<shared_ptr<Object>> objects;
    map<string, shared_ptr<Mapping>> mappings;

public:

    void cleanup();
    void newmap(const string &filename);
    void load(
        const string &filename, const shared_ptr<BxDF> &bxdf, const shared_ptr<Material> &outside,
        const Trans3 &trans = Trans3()
    );
    void import_to(const shared_ptr<Sence> &sence);
};

#ifdef ARC_IMPLEMENTATION

void ObjLoader::cleanup()
{
    objects.clear();
}
void ObjLoader::newmap(const string &filename)
{
    if (!mappings.count(filename))
        mappings[filename] = make_shared<Mapping>(filename);
}
void ObjLoader::load(
    const string &filename, const shared_ptr<BxDF> &bxdf, const shared_ptr<Material> &outside, const Trans3 &trans
)
{
    tinyobj::attrib_t attrib;
    string warn, err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str());
    if (!warn.empty()) std::cerr << warn << std::endl;
    if (!err.empty()) std::cout << err << std::endl;
    if (!ret) return;
    cerr << "shapes : " << shapes.size() << endl;
    for (auto &s : shapes)
    {
        int index_offset = 0;
        for (int f = 0; f < s.mesh.num_face_vertices.size(); ++f)
        {
            int fv = s.mesh.num_face_vertices[f];

            vector<Vec3> V, Vn, Vt;
            for (int v = 0; v < fv; ++v)
            {
                tinyobj::index_t idx = s.mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                // tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                // tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                // tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
                tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];

                V.push_back(trans.apply(Vec3(vx, vy, vz)));
                // Vn.emplace_back(nx, ny, nz);
                Vt.emplace_back(tx, ty, 0);
            }

            index_offset += fv;

            tinyobj::material_t M = materials[s.mesh.material_ids[f]];

            Spectrum emission = rgb(M.emission[0], M.emission[1], M.emission[2]),
                diffuse = rgb(M.diffuse[0], M.diffuse[1], M.diffuse[2]),
                specular = rgb(M.specular[0], M.specular[1], M.specular[2]);

            double prob = emission.norm() / (emission.norm() + diffuse.norm());
            if (prob > EPS)
            {
                emission = emission * (emission.norm() + diffuse.norm());
                diffuse = diffuse * (emission.norm() + diffuse.norm());
                specular = specular * (emission.norm() + diffuse.norm());
            }
            shared_ptr<Material> material = make_shared<Material>(
                prob, emission, M.ior, M.dissolve,
                diffuse, specular,
                Spectrum(1), M.name
            );

            shared_ptr<Shape> shape = make_shared<TriObj>(
                V[0], V[1], V[2]//, Vn[0], Vn[1], Vn[2]
            );

            double rough = 1 - M.shininess / 1000;
            shared_ptr<Surface> surface;
            if (!M.diffuse_texname.empty())
            {
                Trans3 mapper(
                    V[0], V[1], V[2], Vt[0], Vt[1], Vt[2]
                );
                newmap(M.diffuse_texname);
                surface = make_shared<Solid>(
                    material, outside, mappings[M.diffuse_texname], mapper, rough
                );
            }
            else
            {
                surface = make_shared<Solid>(
                    material, outside, rough
                );
            }

            objects.push_back(
                make_shared<Object>(
                    bxdf, shape, surface, "imported_object"
                )
            );
        }
    }
}

void ObjLoader::import_to(const shared_ptr<Sence> &sence)
{
    sence->add_objects(objects);
    cleanup();
}
#endif
#endif /* objloader_hpp */
