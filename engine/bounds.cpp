#include "bounds.h"

#include "window.h"

void Bounds::Draw(uint8_t r, uint8_t g, uint8_t b) const
{
    Window::DrawPrimitive::Rectangle(*this, 1, r, g, b);
}


void CircleBounds::Draw(uint8_t r, uint8_t g, uint8_t b) const
{
    Window::DrawPrimitive::Circle(*this, 1, r, g, b);
}
