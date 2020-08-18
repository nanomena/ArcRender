#ifndef graph_hpp
#define graph_hpp

#include "utils.hpp"
#include "object.hpp"

static int KaDanVisit;

struct KaDanTreeNode
{
    Cuboid box;
    shared_ptr<Object> cen;
    KaDanTreeNode *left, *right;

    void load(
        shared_ptr<Object> objects[], int size, const function<KaDanTreeNode *()> &acquire, int d
    );
    void query(const Ray &ray, shared_ptr<Object> &next, Vec3 &intersect) const;
};

class KaDanTree
{
    vector<KaDanTreeNode> tree;
    KaDanTreeNode *root;
    int cnt;

public:
    explicit KaDanTree(vector<shared_ptr<Object>> objects);

    KaDanTreeNode *acquire();

    void query(const Ray &ray, shared_ptr<Object> &next, Vec3 &intersect) const;
};

#ifdef ARC_IMPLEMENTATION

void KaDanTreeNode::load(
    shared_ptr<Object> objects[], int size, const function<KaDanTreeNode *()> &acquire, int d
)
{
    int mid = size / 2;
    nth_element(
        objects, objects + mid, objects + size,
        [&](const shared_ptr<Object> &a, const shared_ptr<Object> &b)
        {
            Cuboid _a = a->outline(), _b = b->outline();
            return (_a.n.d[d] + _a.x.d[d]) < (_b.n.d[d] + _b.x.d[d]);
        }
    );
    cen = objects[mid];
    if (mid)
    {
        left = acquire();
        left->load(objects, mid, acquire, (d + 1) % 3);
    }
    if (mid + 1 < size)
    {
        right = acquire();
        right->load(objects + mid + 1, size - mid - 1, acquire, (d + 1) % 3);
    }
    box = cen->outline();
    if (left != nullptr) box = box + left->box;
    if (right != nullptr) box = box + right->box;
}

void KaDanTreeNode::query(const Ray &ray, shared_ptr<Object> &next, Vec3 &intersect) const
{
    int is_inter;
    Vec3 candipoint;
    box.inter(ray, is_inter, candipoint);
    if (!is_inter) return;
    if ((candipoint - ray.o).norm2() >= (intersect - ray.o).norm2()) return;

    KaDanVisit++;

    cen->inter(ray, is_inter, candipoint);
    // $ << "ray " << ray << " " << cen->outline() << " " << is_inter << endl;
    if (is_inter)
        if ((candipoint - ray.o).norm2() < (intersect - ray.o).norm2())
        {
            intersect = candipoint;
            next = cen;
        }

    if (left == nullptr && right == nullptr) return;
    if (left == nullptr)
    {
        right->query(ray, next, intersect);
        return;
    }
    if (right == nullptr)
    {
        left->query(ray, next, intersect);
        return;
    }

    int hit_left;
    Vec3 candipoint_l;
    int hit_right;
    Vec3 candipoint_r;
    (left->box).inter(ray, hit_left, candipoint_l);
    (right->box).inter(ray, hit_right, candipoint_r);

    if (!hit_left && !hit_right) return;
    if (!hit_left)
    {
        right->query(ray, next, intersect);
        return;
    }
    if (!hit_right)
    {
        left->query(ray, next, intersect);
        return;
    }

    if ((candipoint_l - ray.o).norm2() < (candipoint_r - ray.o).norm2())
    {
        left->query(ray, next, intersect);
        right->query(ray, next, intersect);
    }
    else
    {
        right->query(ray, next, intersect);
        left->query(ray, next, intersect);
    }
}

KaDanTree::KaDanTree(vector<shared_ptr<Object>> objects)
{
    cerr << "faces : " << objects.size() << endl;
    cnt = 0;
    tree.resize(objects.size());
    root = acquire();
    root->load(&(objects.front()), objects.size(), [this] { return this->acquire(); }, 0);
}

KaDanTreeNode *KaDanTree::acquire() { return &tree[cnt++]; }

void KaDanTree::query(const Ray &ray, shared_ptr<Object> &next, Vec3 &intersect) const
{
    KaDanVisit = 0;
    root->query(ray, next, intersect);
}

#endif
#endif /* graph_hpp */
