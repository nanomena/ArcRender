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
    void newmap(string filename);
    void load(string filename, shared_ptr<BxDF> bxdf);
    void import(shared_ptr<Sence> sence);
};


#ifdef ARC_IMPLEMENTATION

void ObjLoader::cleanup()
{
    objects.clear();
}
void ObjLoader::newmap(string filename)
{
    if (!mappings.count(filename))
        mappings[filename] = make_shared<Mapping>(filename);
}
void ObjLoader::load(string filename, shared_ptr<BxDF> bxdf)
{
    tinyobj::attrib_t attrib;
    string warn, err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str());
    if (!warn.empty()) std::cerr << warn << std::endl;
    if (!err.empty()) std::cout << err << std::endl;
    if (!ret) return;
    cerr << "shapes : " << shapes.size() << endl;
    for (int s = 0; s < shapes.size(); ++ s)
    {
        int index_offset = 0;
        for (int f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++ f)
        {
            int fv = shapes[s].mesh.num_face_vertices[f];

            vector<Vec3> V, Vn, Vt;
            for (int v = 0; v < fv; ++ v)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                // tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                // tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                // tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
                tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];

                V.push_back(Vec3(vx, vy, -vz));
                // Vn.push_back(Vec3(nx, ny, nz));
                Vt.push_back(Vec3(tx, ty, 0));
            }

            index_offset += fv;

            tinyobj::material_t M = materials[shapes[s].mesh.material_ids[f]];

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
                prob,
                emission,
                M.ior, M.dissolve,
                diffuse,
                specular,
                Spectrum(1), M.name
            );

            shared_ptr<Shape> shape = make_shared<TriObj>(
                V[0], V[1], V[2]//, Vn[0], Vn[1], Vn[2]
            );

            double rough = 1 - M.shininess / 1000;
            shared_ptr<Surface> surface;
            if (M.diffuse_texname != "")
            {
                Trans3 trans(
                    V[0], V[1], V[2], Vt[0], Vt[1], Vt[2]
                );
                newmap(M.diffuse_texname);
                surface = make_shared<Thin>(
                    material, mappings[M.diffuse_texname], trans, rough
                );
            }
            else
            {
                surface = make_shared<Thin>(
                    material, rough
                );
            }

            objects.push_back(
                make_shared<Object>(
                    bxdf,
                    shape,
                    surface,
                    "imported_object"
                )
            );
        }
    }
}

void ObjLoader::import(shared_ptr<Sence> sence)
{
    for (auto object : objects) sence->add_object(object);
    cleanup();
}
#endif
#endif /* objloader_hpp */
