#include "vec.h"

#include <vector>
#include "debug.h"
#include "bounds.h"
#include "window.h"

const vec vec::Zero(0.f, 0.f);

#ifdef _DEBUG
struct debugvec {
    debugvec(vec v, uint8_t r, uint8_t g, uint8_t b) : v(v), r(r), g(g), b(b) {}
    vec v;
    int r,g,b;
    vec from; //only used for arrows
};

std::vector<debugvec> debugvecs;
std::vector<debugvec> debugarrows;

void vec::DebuggerinoAsArrow(vec from, uint8_t r, uint8_t g, uint8_t b) const {
    debugarrows.emplace_back(*this, r, g, b).from = from;
}
void vec::Debuggerino(uint8_t r, uint8_t g, uint8_t b) const {
    debugvecs.emplace_back(*this, r, g, b);
}

void DrawDebugVecs() 
{
    for (const debugvec& v : debugvecs) {
        Bounds::fromCenter(v.v, vec(4, 4)).Draw(v.r,v.g,v.b);
    }
    for (const debugvec& v : debugarrows) {
        Window::DrawPrimitive::Line(v.from, v.from + v.v, 1, v.r, v.g, v.b);
        vec toTheSide = (v.v).Perp().Normalized()*3.f;
        Window::DrawPrimitive::Line(v.from + v.v, v.from + v.v - toTheSide.RotatedAroundOriginDegs(-45), 1, v.r, v.g, v.b);
        Window::DrawPrimitive::Line(v.from + v.v, v.from + v.v + toTheSide.RotatedAroundOriginDegs(45), 1, v.r, v.g, v.b);
    }
}

void ClearDebugVecs() {
    debugvecs.clear();
    debugarrows.clear();
}

#endif