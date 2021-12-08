#ifndef graph_hpp
#define graph_hpp

#include "utils.hpp"
#include "object.hpp"

static int KaDanVisit;

struct KaDanTreeNode {
    Box3 box;
    shared_ptr<Object> cen;
    KaDanTreeNode *left, *right;

    void load(
        shared_ptr<Object> objects[], int size, const function<KaDanTreeNode *()> &acquire, int d
    );
    void intersect(const Ray &ray, shared_ptr<Object> &object, double &t) const;
};

class KaDanTree {
    vector<KaDanTreeNode> tree;
    KaDanTreeNode *root;
    int cnt;

public:
    explicit KaDanTree(vector<shared_ptr<Object>> objects);

    KaDanTreeNode *acquire();

    void intersect(const Ray &ray, shared_ptr<Object> &object, double &t) const;
};

#ifdef ARC_IMPLEMENTATION

void KaDanTreeNode::load(
    shared_ptr<Object> objects[], int size, const function<KaDanTreeNode *()> &acquire, int d
) {
    int mid = size / 2;
    nth_element(
        objects, objects + mid, objects + size,
        [&](const shared_ptr<Object> &a, const shared_ptr<Object> &b) {
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

void KaDanTreeNode::intersect(const Ray &ray, shared_ptr<Object> &object, double &t) const {
    int is_inter;
    double t_cur;
    if (!box.intersect(ray, t_cur)) return;
    if (t_cur >= t) return;

    KaDanVisit++;

    // $ << "ray " << ray << " " << cen->outline() << " " << is_inter << endl;
    if (cen->intersect(ray, t_cur))
        if (t_cur < t) {
            t = t_cur;
            object = cen;
        }

    bool hit_l, hit_r;
    double t_l, t_r;
    hit_l = left != nullptr && (left->box).intersect(ray, t_l);
    hit_r = right != nullptr && (right->box).intersect(ray, t_r);

    if (!hit_l && !hit_r) return;
    if (!hit_l) {
        right->intersect(ray, object, t);
        return;
    }
    if (!hit_r) {
        left->intersect(ray, object, t);
        return;
    }

    if (t_l < t_r) {
        left->intersect(ray, object, t);
        right->intersect(ray, object, t);
    } else {
        right->intersect(ray, object, t);
        left->intersect(ray, object, t);
    }
}

KaDanTree::KaDanTree(vector<shared_ptr<Object>> objects) {
    cerr << "faces : " << objects.size() << endl;
    cnt = 0;
    tree.resize(objects.size());
    root = acquire();
    root->load(&(objects.front()), objects.size(), [this] { return this->acquire(); }, 0);
}

KaDanTreeNode *KaDanTree::acquire() { return &tree[cnt++]; }

void KaDanTree::intersect(const Ray &ray, shared_ptr<Object> &object, double &t) const {
    KaDanVisit = 0;
    root->intersect(ray, object, t);
}

#endif
#endif /* graph_hpp */
