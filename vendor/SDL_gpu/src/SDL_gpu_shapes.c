#include "SDL_gpu.h"
#include "SDL_gpu_RendererImpl.h"
#include <string.h>

#ifdef _MSC_VER
// Disable warning: selection for inlining
#pragma warning(disable: 4514 4711)
// Disable warning: Spectre mitigation
#pragma warning(disable: 5045)
#endif

#define CHECK_RENDERER() \
GPU_Renderer* renderer = GPU_GetCurrentRenderer(); \
if(renderer == NULL) \
    return;

#define CHECK_RENDERER_1(ret) \
GPU_Renderer* renderer = GPU_GetCurrentRenderer(); \
if(renderer == NULL) \
    return ret;


float GPU_SetLineThickness(float thickness)
{
	CHECK_RENDERER_1(1.0f);
	return renderer->impl->SetLineThickness(renderer, thickness);
}

float GPU_GetLineThickness(void)
{
	CHECK_RENDERER_1(1.0f);
	return renderer->impl->GetLineThickness(renderer);
}

void GPU_Pixel(GPU_Target* target, float x, float y, float z, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->Pixel(renderer, target, x, y, z, color);
}

void GPU_Line(GPU_Target* target, float x1, float y1, float x2, float y2, float z, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->Line(renderer, target, x1, y1, x2, y2, z, color);
}


void GPU_Arc(GPU_Target* target, float x, float y, float radius, float start_angle, float end_angle, float z, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->Arc(renderer, target, x, y, z, radius, start_angle, end_angle, color);
}


void GPU_ArcFilled(GPU_Target* target, float x, float y, float radius, float start_angle, float end_angle, float z, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->ArcFilled(renderer, target, x, y, z, radius, start_angle, end_angle, color);
}

void GPU_Circle(GPU_Target* target, float x, float y, float z, float radius, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->Circle(renderer, target, x, y, z, radius, color);
}

void GPU_CircleFilled(GPU_Target* target, float x, float y, float z, float radius, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->CircleFilled(renderer, target, x, y, z, radius, color);
}

void GPU_Ellipse(GPU_Target* target, float x, float y, float rx, float ry, float z, float degrees, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->Ellipse(renderer, target, x, y, rx, ry, z, degrees, color);
}

void GPU_EllipseFilled(GPU_Target* target, float x, float y, float rx, float ry, float z, float degrees, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->EllipseFilled(renderer, target, x, y, rx, ry, z, degrees, color);
}

void GPU_Sector(GPU_Target* target, float x, float y, float z, float inner_radius, float outer_radius, float start_angle, float end_angle, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->Sector(renderer, target, x, y, z, inner_radius, outer_radius, start_angle, end_angle, color);
}

void GPU_SectorFilled(GPU_Target* target, float x, float y, float z, float inner_radius, float outer_radius, float start_angle, float end_angle, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->SectorFilled(renderer, target, x, y, z, inner_radius, outer_radius, start_angle, end_angle, color);
}

void GPU_Tri(GPU_Target* target, float x1, float y1, float x2, float y2, float x3, float y3, float z, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->Tri(renderer, target, x1, y1, x2, y2, x3, y3, z, color);
}

void GPU_TriFilled(GPU_Target* target, float x1, float y1, float x2, float y2, float x3, float y3, float z, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->TriFilled(renderer, target, x1, y1, x2, y2, x3, y3, z, color);
}

void GPU_Rectangle(GPU_Target* target, float x1, float y1, float x2, float y2, float z, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->Rectangle(renderer, target, x1, y1, x2, y2, z, color);
}

void GPU_Rectangle2(GPU_Target* target, GPU_Rect rect, float z, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->Rectangle(renderer, target, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, z, color);
}

void GPU_RectangleFilled(GPU_Target* target, float x1, float y1, float x2, float y2, float z, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->RectangleFilled(renderer, target, x1, y1, x2, y2, z, color);
}

void GPU_RectangleFilled2(GPU_Target* target, GPU_Rect rect, float z, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->RectangleFilled(renderer, target, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, z, color);
}

void GPU_RectangleRound(GPU_Target* target, float x1, float y1, float x2, float y2, float z, float radius, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->RectangleRound(renderer, target, x1, y1, x2, y2, z, radius, color);
}

void GPU_RectangleRound2(GPU_Target* target, GPU_Rect rect, float z, float radius, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->RectangleRound(renderer, target, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, z, radius, color);
}

void GPU_RectangleRoundFilled(GPU_Target* target, float x1, float y1, float x2, float y2, float z, float radius, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->RectangleRoundFilled(renderer, target, x1, y1, x2, y2, z, radius, color);
}

void GPU_RectangleRoundFilled2(GPU_Target* target, GPU_Rect rect, float z, float radius, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->RectangleRoundFilled(renderer, target, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, z, radius, color);
}

void GPU_Polygon(GPU_Target* target, unsigned int num_vertices, float* vertices, float z, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->Polygon(renderer, target, num_vertices, vertices, z, color);
}

void GPU_Polyline(GPU_Target* target, unsigned int num_vertices, float* vertices, float z, SDL_Color color, GPU_bool close_loop)
{
	CHECK_RENDERER();
	renderer->impl->Polyline(renderer, target, num_vertices, vertices, z, color, close_loop );
}

void GPU_PolygonFilled(GPU_Target* target, unsigned int num_vertices, float* vertices, float z, SDL_Color color)
{
	CHECK_RENDERER();
	renderer->impl->PolygonFilled(renderer, target, num_vertices, vertices, z, color);
}

