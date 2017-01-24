#ifndef STRUCT_H
#define STRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <vector>
#include "types.h"

#define DIM 1024

I visited;

using namespace std;

struct kd_node_t{
    vector<D> x;
    struct kd_node_t *left, *right;
};



D dist(struct kd_node_t *a, struct kd_node_t *b)
{
    D d = 0;
    for(I i=0;i<DIM;++i){
        d+=pow(b->x[i]-a->x[i],2);
    }
    return d;
}


void swap(struct kd_node_t *x, struct kd_node_t *y) {
    x->x.swap(y->x);
}


struct kd_node_t* find_median(struct kd_node_t *start, struct kd_node_t *end, I idx)
{
    if (end <= start) return NULL;
    if (end == start + 1)
        return start;

    struct kd_node_t *p, *store, *md = start + (end - start) / 2;
    D pivot;
    while (1) {
        pivot = md->x[idx];

        swap(md, end - 1);
        for (store = p = start; p < end; p++) {
            if (p->x[idx] < pivot) {
                if (p != store)
                    swap(p, store);
                store++;
            }
        }
        swap(store, end - 1);

        if (store->x[idx] == md->x[idx])
            return md;

        if (store > md) end = store;
        else        start = store;
    }
}

struct kd_node_t* make_tree(struct kd_node_t *t, I len, I i, I dim)
//struct kd_node_t* make_tree(vector<kd_node_t> &t, I len, I i, I dim)
{
    struct kd_node_t *n;

    if (!len) return 0;

    //qDebug((to_string((t+54)->x[9])).c_str());

    if ((n = find_median(t, t + len, i))) {
        i = (i + 1) % dim;
        n->left  = make_tree(t, n - t, i, dim);
        n->right = make_tree(n + 1, t + len - (n + 1), i, dim);
    }
    return n;
}

void nearest(struct kd_node_t *root, struct kd_node_t *nd, I i, I dim, struct kd_node_t **best, D *best_dist)
{
    D d, dx, dx2;

    if (!root) return;
    d = dist(root, nd);
    dx = root->x[i] - nd->x[i];
    dx2 = dx * dx;

    visited ++;

    if (!*best || d < *best_dist) {
        *best_dist = d;
        *best = root;
    }

    /* if chance of exact match is high */
    if (!*best_dist) return;

    if (++i >= dim) i = 0;

    nearest(dx > 0 ? root->left : root->right, nd, i, dim, best, best_dist);
    if (dx2 >= *best_dist) return;
    nearest(dx > 0 ? root->right : root->left, nd, i, dim, best, best_dist);
}

#endif // STRUCT_H
