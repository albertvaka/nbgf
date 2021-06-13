#include "City.h"
#include "camera.h"
#include "assets.h"
#include "debug.h"
#include "Tree.h"
#include "vec.h"
#include "window_conf.h"


City::City(): mTrees() {
    //mEntities.push_back(new Tree(vec(100, 100)));
    mTrees.push_back(new Tree(vec(200,200)));
    mTrees.push_back(new Tree(vec(500,400)));
}
City::~City() {
    /*
    for(auto* e : mEntities) {
        delete e;
    }
    */
    for(Tree* t : mTrees) {
        delete t;
    }
}

void City::Draw() {
    /*
    for(auto& e : mEntities) {
        e->Draw();
    }
    */
    for(Tree* t : mTrees) {
        t->Draw();
    }
}