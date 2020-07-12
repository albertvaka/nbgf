#include "vec.h"

#include <vector>
#include "debug.h"
#include "bounds.h"

const vec vec::Zero(0.f, 0.f);

#ifdef _DEBUG
struct debugvec {
    debugvec(const vec& v, uint8_t r, uint8_t g, uint8_t b) : v(v), r(r), g(g), b(b) {}
    vec v; 
    int r,g,b;
};

std::vector<debugvec> debugvecs;

void vec::Debuggerino(uint8_t r, uint8_t g, uint8_t b) const {
    debugvecs.emplace_back(*this, r, g, b);
}

void DrawDebugVecs() 
{
    for (debugvec v : debugvecs) {
        Bounds::fromCenter(v.v, vec(4, 4)).Draw(v.r,v.g,v.b);
    }
}

void ClearDebugVecs() {
    debugvecs.clear();
}

#endif