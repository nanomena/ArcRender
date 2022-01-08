#ifndef graph_hpp
#define graph_hpp

#include "utils.hpp"
#include "shape.hpp"

static int KaDanVisit;

struct KaDanTreeNode {
    Box3 box;
    const Shape *cen{};
    KaDanTreeNode *left{}, *right{};

    void load(
        const Shape *objects[], unsigned int size, const function<KaDanTreeNode *()> &acquire, int d
    );
    void intersect(const Ray &ray, const Shape *&object, double &t, Vec3 &pos, Vec2 &texPos) const;
};

class KaDanTree {
public:
    explicit KaDanTree(vector<const Shape *> objects);

    KaDanTreeNode *acquire();

    void intersect(const Ray &ray, const Shape *&object, double &t, Vec3 &pos, Vec2 &texPos) const;
    Box3 box() const;

private:
    vector<KaDanTreeNode> tree;
    KaDanTreeNode *root;
    int cnt;
};

#ifdef ARC_IMPLEMENTATION

void KaDanTreeNode::load(
    const Shape *objects[], unsigned int size, const function<KaDanTreeNode *()> &acquire, int d
) {
    unsigned int mid = size / 2;
    nth_element(
        objects, objects + mid, objects + size,
        [&](const Shape *a, const Shape *b) {
            Box3 _a = a->outline(), _b = b->outline();
            return (_a.L()[d] + _a.U()[d]) < (_b.L()[d] + _b.U()[d]);
        }
    );
    cen = objects[mid];
    if (mid) {
        left = acquire();
        left->load(objects, mid, acquire, (d + 1) % 3);
    }
    if (mid + 1 < size) {
        right = acquire();
        right->load(objects + mid + 1, size - mid - 1, acquire, (d + 1) % 3);
    }
    box = cen->outline();
    if (left != nullptr) box = box + left->box;
    if (right != nullptr) box = box + right->box;
}

void KaDanTreeNode::intersect(const Ray &ray, const Shape *&object, double &t, Vec3 &pos, Vec2 &texPos) const {
    int is_inter;
    double tCur;
    if (!box.intersect(ray, tCur)) return;
    if (tCur >= t) return;

//    KaDanVisit++;

    // $ << "ray " << ray << " " << cen->outline() << " " << is_inter << endl;
    Vec3 posCur; Vec2 texPosCur;
    if (cen->intersect(ray, tCur, posCur, texPosCur))
        if (tCur < t) {
            t = tCur;
            pos = posCur;
            texPos = texPosCur;
            object = cen;
        }

    bool hit_l, hit_r;
    double t_l, t_r;
    hit_l = left != nullptr && (left->box).intersect(ray, t_l);
    hit_r = right != nullptr && (right->box).intersect(ray, t_r);

    if (!hit_l && !hit_r) return;
    if (!hit_l) {
        right->intersect(ray, object, t, pos, texPos);
        return;
    }
    if (!hit_r) {
        left->intersect(ray, object, t, pos, texPos);
        return;
    }

    if (t_l < t_r) {
        left->intersect(ray, object, t, pos, texPos);
        right->intersect(ray, object, t, pos, texPos);
    } else {
        right->intersect(ray, object, t, pos, texPos);
        left->intersect(ray, object, t, pos, texPos);
    }
}

KaDanTree::KaDanTree(vector<const Shape *> objects) {
    cerr << "faces : " << objects.size() << endl;
    cnt = 0;
    tree.resize(objects.size());
    root = acquire();
    root->load(&(objects.front()), objects.size(), [this] { return this->acquire(); }, 0);
    cerr << "box : " << root->box.L() << " " << root->box.U() << endl;
}

KaDanTreeNode *KaDanTree::acquire() { return &tree[cnt++]; }

void KaDanTree::intersect(const Ray &ray, const Shape *&object, double &t, Vec3 &pos, Vec2 &texPos) const {
//    KaDanVisit = 0;
    root->intersect(ray, object, t, pos, texPos);
//#pragma omp critical
//    cerr << KaDanVisit << endl;
//    for (const auto& node : tree) {
//        double t_cur;
//        if (node.cen->intersect(ray, t_cur))
//            if (t_cur < t) {
//                t = t_cur;
//                object = node.cen;
//            }
//    }
}

Box3 KaDanTree::box() const {
    return root->box;
}

#endif
#endif /* graph_hpp */
