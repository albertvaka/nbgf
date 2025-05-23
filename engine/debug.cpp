#include "debug.h"

#include <iostream>
#include <vector>
#include <chrono>

#include "vec.h"
#include "bounds.h"
#include "window_drawprimitive.h"

namespace Debug {
	DebugStream out;
	std::ostream& _forwarded_out = std::cout;
}

DebugStreamDelegate::~DebugStreamDelegate() {
	Debug::_forwarded_out << std::endl;
}

#ifdef _DEBUG

// DebugDraw stuff  ------------------------------

struct debugvec {
    debugvec(vec v, uint8_t r, uint8_t g, uint8_t b) : v(v), color({ r,g,b,255 }), radius(-1) {}
    debugvec(){} // std::vector wants this
    vec v;
    SDL_Color color;
    float radius; //only used for circles
    vec from; //only used for arrows and boxes
};

std::vector<debugvec> debugvecs;
std::vector<debugvec> debugarrows;
std::vector<debugvec> debugsegments;
std::vector<debugvec> debugbounds;
bool inSceneDraw = false;
int debugvecs_before_draw = -1;
int debugarrows_before_draw = -1;
int debugsegments_before_draw = -1;
int debugbounds_before_draw = -1;

void BoxBounds::DebugDraw(uint8_t r, uint8_t g, uint8_t b) const
{
    if (inSceneDraw && !Debug::Draw) return; // we still need to queue draws done in the Update because they might not happen again if we enter frame-to-frame
    debugbounds.emplace_back(TopLeft(), r, g, b).from = BottomRight();
}

void CircleBounds::DebugDraw(uint8_t r, uint8_t g, uint8_t b) const
{
    if (inSceneDraw && !Debug::Draw) return;
    debugbounds.emplace_back(Center(), r, g, b).radius = radius;
}

void vec::DebugDrawAsArrow(vec from, uint8_t r, uint8_t g, uint8_t b) const
{
    if (inSceneDraw && !Debug::Draw) return;
    debugarrows.emplace_back(*this, r, g, b).from = from;
}

void vec::DebugDrawAsSegment(vec from, uint8_t r, uint8_t g, uint8_t b) const
{
    if (inSceneDraw && !Debug::Draw) return;
    debugsegments.emplace_back(*this, r, g, b).from = from;
}

void vec::DebugDraw(uint8_t r, uint8_t g, uint8_t b) const 
{
    if (inSceneDraw && !Debug::Draw) return;
    debugvecs.emplace_back(*this, r, g, b);
}

void BeforeSceneUpdate() 
{
    debugvecs.clear();
    debugarrows.clear();
    debugbounds.clear();
}

void BeforeSceneDraw() 
{
    inSceneDraw = true;
    debugvecs_before_draw = debugvecs.size();
    debugarrows_before_draw = debugarrows.size();
    debugbounds_before_draw = debugbounds.size();
    debugsegments_before_draw = debugsegments.size();
}

void AfterSceneDraw()
{
    inSceneDraw = false;

    //Debug::out << std::to_string(debugvecs.size() + debugarrows.size() + debugbounds.size());

    if (Debug::Draw) {

        for (const debugvec& v : debugvecs) {
            Window::DrawPrimitive::Rectangle(v.v - vec(2, 2) * Debug::DebugDrawScale, v.v + vec(2, 2) * Debug::DebugDrawScale, 1*Debug::DebugDrawScale, v.color);
        }
        for (const debugvec& v : debugsegments) {
            Window::DrawPrimitive::Line(v.from, v.v, 3 * Debug::DebugDrawScale, v.color);
        }
        for (const debugvec& v : debugarrows) {
            Window::DrawPrimitive::Arrow(v.from, v.from + v.v, 1 * Debug::DebugDrawScale, 3 * Debug::DebugDrawScale, v.color);
        }
        for (const debugvec& v : debugbounds) {
            if (v.radius < 0) {
                Window::DrawPrimitive::Rectangle(v.v, v.from, 1 * Debug::DebugDrawScale, v.color);
            }
            else {
                Window::DrawPrimitive::Circle(v.v, v.radius, 1 * Debug::DebugDrawScale, v.color);
            }
        }
    }

    // shrink to erase draws added after the Update, since they will be added again next frame
    debugvecs.resize(debugvecs_before_draw);
    debugarrows.resize(debugarrows_before_draw);
    debugsegments.resize(debugsegments_before_draw);
    debugbounds.resize(debugbounds_before_draw);
}

#endif

namespace Debug {
    void timed(std::function<void()> f) {
        auto startTime = std::chrono::system_clock::now().time_since_epoch();
        f();
        auto elapsedTime = std::chrono::system_clock::now().time_since_epoch() - startTime;
        int elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();
        if (elapsedMs != 0) {
            Debug::out << "Time: " << elapsedMs << "ms";
        } else {
            int elapsedNs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
            Debug::out << "Time: " << (elapsedNs/1000.0) << "ms";
        }
    }
}
