#ifndef objsence_hpp
#define objsence_hpp

#include "utils.hpp"
#include "geometry.hpp"
#include "object.hpp"
#include "sence.hpp"


int decompose(char st[], int &a, int &b, int &c)
{
    int p = 0; a = b = c = 0;
    while (isspace(st[p])) p ++;
    if (st[p] == 0) return -1; else if (st[p] == '-') p ++;
    while (isdigit(st[p])) a = a * 10 + (st[p ++] - '0');
    if (st[p] == '/') p ++; else if (st[p] == '-') p ++; else return p;
    while (isdigit(st[p])) b = b * 10 + (st[p ++] - '0');
    if (st[p] == '/') p ++; else if (st[p] == '-') p ++; else return p;
    while (isdigit(st[p])) c = c * 10 + (st[p ++] - '0');
    return p;
}

class ObjSence
{
    vector<Vec3> V, Vt, Vn;
    vector<shared_ptr<Object>> objects;

public:
    void import(const char* path, shared_ptr<Sence> sence, const Mat3 &T)
    {
        int num = 0;
        shared_ptr<Surface> light05 = make_shared<OneFaceLightSource>(Spectrum(0.5));
        shared_ptr<Surface> light0 = make_shared<LightSource>(
            Spectrum(0)
        );
        shared_ptr<Surface> diffuse_surface = make_shared<MetalSurface>(
            rgb888(230), 0.5, 0.5
        );
        shared_ptr<Material> emerald = make_shared<Material>(
            1.6, Spectrum(0.97, 0.995, 0.97), "emerald"
        );

        V.clear(); Vt.clear(); Vn.clear(); objects.clear();
        V.push_back(Vec3());
        Vt.push_back(Vec3());
        Vn.push_back(Vec3());

        ifstream fin(path);
        const int SIZE = 1024;
        char buffer[SIZE];
        while (fin.getline(buffer, SIZE))
        {
            if (buffer[0] == 'v')
            {
                double x, y, z;
                if (buffer[1] == ' ')
                {
                    sscanf(buffer + 2, "%lf%lf%lf", &x, &y, &z);
                    V.push_back(Vec3(x, y, z));
                }
            }
            else if (buffer[0] == 'f')
            {
                int p = 1;
                vector<int> v, vt, vn;
                while (1)
                {
                    int _v, _vt, _vn;
                    int d = decompose(buffer + p, _v, _vt, _vn);
                    if (d == -1) break; else p += d;
                    v.push_back(_v);
                    vt.push_back(_vt);
                    vn.push_back(_vn);
                }
                for (int i = 1; i + 1 < v.size(); ++ i)
                {
                    shared_ptr<Shape> now = make_shared<TriObj>(
                        T * V[v[0]], T * V[v[i]], T * V[v[i + 1]]
                    );
                    // shared_ptr<Shape> now = make_shared<Flat>(
                    //     3, T * V[v[0]], T * V[v[i]], T * V[v[i + 1]]
                    // );
                    // shared_ptr<Shape> now = make_shared<TriObj>(
                    //     T * V[v[0]], T * Vn[vn[0]], T * V[v[i]], T * Vn[vn[i]], T * V[v[i + 1]], T * Vn[vn[i + 1]]
                    // );
                    // $ << V[v[0]] << " " << V[v[i]] << " " << V[v[i + 1]] << endl;

                    sence->add_object(make_shared<Object>(
                        now, make_shared<TransSurface>(emerald, 0.1), string(path) + to_string(num)
                    ));
                }
                num ++;
            }
        }
        V.clear(); Vn.clear(); Vt.clear();
    }
};


#endif
