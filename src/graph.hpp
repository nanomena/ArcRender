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

    void load(shared_ptr<Object> objects[], int size, \
              function<KaDanTreeNode*()> acquire, int d);
    void query(const Ray &ray, shared_ptr<Object> &next, Vec3 &hitpoint) const;
};

class KaDanTree
{
    vector<KaDanTreeNode> tree;
    KaDanTreeNode *root;
    int cnt;

public:
    KaDanTree ();

    KaDanTreeNode* acquire();

    void load(shared_ptr<Object> objects[], int size);
    void query(const Ray &ray, shared_ptr<Object> &next, Vec3 &hitpoint) const;
};

#ifdef ARC_IMPLEMENTATION

void KaDanTreeNode::load(shared_ptr<Object> objects[], int size, \
        function<KaDanTreeNode*()> acquire, int d)
{
    int mid = size / 2;
    nth_element(objects, objects + mid, objects + size,
        [&](shared_ptr<Object> a, shared_ptr<Object> b)
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

void KaDanTreeNode::query(const Ray &ray, shared_ptr<Object> &next, Vec3 &hitpoint) const
{
    int hit; Vec3 candipoint;
    box.inter(ray, hit, candipoint);
    if (!hit) return;
    if ((candipoint - ray.o).norm2() >= (hitpoint - ray.o).norm2()) return;

    KaDanVisit ++;

    cen->inter(ray, hit, candipoint);
    // $ << "ray " << ray << " " << cen->outline() << " " << hit << endl;
    if (hit) if ((candipoint - ray.o).norm2() < (hitpoint - ray.o).norm2())
    {
        hitpoint = candipoint;
        next = cen;
    }

    if (left == nullptr && right == nullptr) return;
    if (left == nullptr) { right->query(ray, next, hitpoint); return; }
    if (right == nullptr) { left->query(ray, next, hitpoint); return; }

    int hit_left; Vec3 candipoint_l;
    int hit_right; Vec3 candipoint_r;
    (left->box).inter(ray, hit_left, candipoint_l);
    (right->box).inter(ray, hit_right, candipoint_r);

    if (!hit_left && !hit_right) return;
    if (!hit_left) { right->query(ray, next, hitpoint); return; }
    if (!hit_right) { left->query(ray, next, hitpoint); return; }

    if ((candipoint_l - ray.o).norm2() < (candipoint_r - ray.o).norm2())
    {
        left->query(ray, next, hitpoint);
        right->query(ray, next, hitpoint);
    }
    else
    {
        right->query(ray, next, hitpoint);
        left->query(ray, next, hitpoint);
    }
}

KaDanTree::KaDanTree ()
{
    cnt = 0;
}

KaDanTreeNode* KaDanTree::acquire() { return &tree[cnt ++]; }

void KaDanTree::load(shared_ptr<Object> objects[], int size)
{
    cerr << "faces : " << size << endl;
    tree.resize(size);
    root = acquire();
    root->load(objects, size, bind(&KaDanTree::acquire, this), 0);
}
void KaDanTree::query(const Ray &ray, shared_ptr<Object> &next, Vec3 &hitpoint) const
{
    KaDanVisit = 0;
    root->query(ray, next, hitpoint);
}

#endif
#endif /* graph_hpp */
