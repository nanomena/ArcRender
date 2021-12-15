#ifndef tree_hpp
#define tree_hpp

#include "utils.hpp"
#include "photon.hpp"

static int KaTenVisit;

struct KaTenTreeNode {
    Box3 box;
    shared_ptr<Photon> cen;
    KaTenTreeNode *left, *right;

    void load(
        shared_ptr<Photon> photons[], unsigned int size, const function<KaTenTreeNode *()> &acquire, int d
    );
    void search(const Vec3 &p, double r, const function<void(shared_ptr<Photon> cen)> &func) const;
};

class KaTenTree {
public:
    explicit KaTenTree(vector<shared_ptr<Photon>> photons);

    KaTenTreeNode *acquire();

    void search(const Vec3 &p, double r, const function<void(shared_ptr<Photon> cen)> &func) const;

private:
    vector<KaTenTreeNode> tree;
    KaTenTreeNode *root;
    int cnt;
};

#ifdef ARC_IMPLEMENTATION

void KaTenTreeNode::load(
    shared_ptr<Photon> photons[], unsigned int size, const function<KaTenTreeNode *()> &acquire, int d
) {
    unsigned int mid = size / 2;
    nth_element(
        photons, photons + mid, photons + size,
        [&](const shared_ptr<Photon> &a, const shared_ptr<Photon> &b) {
            return a->intersect[d] < b->intersect[d];
        }
    );
    cen = photons[mid];
    if (mid) {
        left = acquire();
        left->load(photons, mid, acquire, (d + 1) % 3);
    }
    if (mid + 1 < size) {
        right = acquire();
        right->load(photons + mid + 1, size - mid - 1, acquire, (d + 1) % 3);
    }
    box = Box3(cen->intersect);
    if (left != nullptr) box = box + left->box;
    if (right != nullptr) box = box + right->box;
}

void KaTenTreeNode::search(const Vec3 &p, double r, const function<void(shared_ptr<Photon> cen)> &func) const {
    if (!box.intersectCheck(p, r)) return;
    KaTenVisit++;
    if ((cen->intersect - p).squaredLength() < r * r) func(cen);
    if (left != nullptr) left->search(p, r, func);
    if (right != nullptr) right->search(p, r, func);
}

KaTenTree::KaTenTree(vector<shared_ptr<Photon>> photons) {
    cerr << "photons : " << photons.size() << endl;
    cnt = 0;
    tree.resize(photons.size());
    root = acquire();
    root->load(&(photons.front()), photons.size(), [this] { return this->acquire(); }, 0);
}

KaTenTreeNode *KaTenTree::acquire() { return &tree[cnt++]; }

void KaTenTree::search(const Vec3 &p, double r, const function<void(shared_ptr<Photon> cen)> &func) const {
    KaTenVisit = 0;
    root->search(p, r, func);
//    cerr << KaTenVisit << endl;
//    for (const auto &node: tree) {
//        if ((node.cen->intersect - p).squaredLength() < r * r) func(node.cen);
//    }
}

#endif
#endif /* tree_hpp */
