#pragma once

#include "SDL_gpu.h"

namespace Window {

	extern GPU_Target* currentDrawTarget;

	namespace DrawRaw {

		const unsigned short MAX_VERTICES = 60000; //From SDL_GPU's GPU_BLIT_BUFFER_ABSOLUTE_MAX_VERTICES
		const unsigned int MAX_INDICES = (MAX_VERTICES / 4) * 6; //6 indices for each 4 vertices (quads drawn as 2 triangles)
		const unsigned int MAX_COMPONENTS_PER_VERTEX = 8; // Max of GPU_BATCH_XY_ST (4), GPU_BATCH_XY_RGB (5) and GPU_BATCH_XY_ST_RGBA (8) 

		extern unsigned short vertex_count;
		extern unsigned int index_count;
		extern float vertices[MAX_VERTICES * MAX_COMPONENTS_PER_VERTEX];
		extern unsigned short indices[MAX_INDICES];

		inline void FlushTexturedQuads(GPU_Image* t) {
			//Debug::out << "vertices:" << vertex_count << " indices:" << index_count;
			GPU_TriangleBatch(t, Window::currentDrawTarget, vertex_count, vertices, index_count, indices, GPU_BATCH_XY_ST);
			vertex_count = 0;
			index_count = 0;
		}

		inline void FlushRGBQuads() {
			//Debug::out << "vertices:" << vertex_count << " indices:" << index_count;
			GPU_TriangleBatch(NULL, Window::currentDrawTarget, vertex_count, vertices, index_count, indices, GPU_BATCH_XY_RGB);
			vertex_count = 0;
			index_count = 0;
		}

		inline void FlushColoredTexturedQuads(GPU_Image* t) {
			//Debug::out << "vertices:" << vertex_count << " indices:" << index_count;
			GPU_TriangleBatch(t, Window::currentDrawTarget, vertex_count, vertices, index_count, indices, GPU_BATCH_XY_ST_RGBA);
			vertex_count = 0;
			index_count = 0;
		}

		//Colors and texture coords between 0 and 1
		inline void BatchColoredTexturedQuad(GPU_Image* t, float x, float y, float w, float h, const GPU_Rect& tr, float r, float g, float b, float a)
		{
			const int COMPONENTS_PER_VERTEX = 8; // GPU_BATCH_XY_ST_RGBA
			unsigned int i = vertex_count * COMPONENTS_PER_VERTEX;

			//bottom left
			vertices[i++] = x;
			vertices[i++] = y + h;
			vertices[i++] = tr.x;
			vertices[i++] = tr.y + tr.h;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;
			vertices[i++] = a;

			//top left
			vertices[i++] = x;
			vertices[i++] = y;
			vertices[i++] = tr.x;
			vertices[i++] = tr.y;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;
			vertices[i++] = a;

			//top right
			vertices[i++] = x + w;
			vertices[i++] = y;
			vertices[i++] = tr.x + tr.w;
			vertices[i++] = tr.y;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;
			vertices[i++] = a;

			//bottom right
			vertices[i++] = x + w;
			vertices[i++] = y + h;
			vertices[i++] = tr.x + tr.w;
			vertices[i++] = tr.y + tr.h;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;
			vertices[i++] = a;

			indices[index_count++] = vertex_count;
			indices[index_count++] = vertex_count + 1;
			indices[index_count++] = vertex_count + 2;
			indices[index_count++] = vertex_count;
			indices[index_count++] = vertex_count + 2;
			indices[index_count++] = vertex_count + 3;

			vertex_count += 4;

			if (vertex_count + 4 >= MAX_VERTICES) {
				// Flush what we have so we don't go over MAX_VERTICES
				FlushColoredTexturedQuads(t);
			};
		}
		
		//Texture coords between 0 and 1
		inline void BatchTexturedQuad(GPU_Image* t, float x, float y, float w, float h, const GPU_Rect& tr)
		{
			const int COMPONENTS_PER_VERTEX = 4; // GPU_BATCH_XY_ST
			unsigned int i = vertex_count * COMPONENTS_PER_VERTEX;

			//bottom left
			vertices[i++] = x;
			vertices[i++] = y + h;
			vertices[i++] = tr.x;
			vertices[i++] = tr.y + tr.h;

			//top left
			vertices[i++] = x;
			vertices[i++] = y;
			vertices[i++] = tr.x;
			vertices[i++] = tr.y;

			//top right
			vertices[i++] = x + w;
			vertices[i++] = y;
			vertices[i++] = tr.x + tr.w;
			vertices[i++] = tr.y;

			//bottom right
			vertices[i++] = x + w;
			vertices[i++] = y + h;
			vertices[i++] = tr.x + tr.w;
			vertices[i++] = tr.y + tr.h;

			indices[index_count++] = vertex_count;
			indices[index_count++] = vertex_count + 1;
			indices[index_count++] = vertex_count + 2;
			indices[index_count++] = vertex_count;
			indices[index_count++] = vertex_count + 2;
			indices[index_count++] = vertex_count + 3;

			vertex_count += 4;

			if (vertex_count + 4 >= MAX_VERTICES) {
				// Flush what we have so we don't go over MAX_VERTICES
				FlushTexturedQuads(t);
			};
		}

		//Colors between 0 and 1
		inline void BatchRGBQuad(float x, float y, float width, float height, float r, float g, float b)
		{
			const int COMPONENTS_PER_VERTEX = 5; // GPU_BATCH_XY_RGB
			unsigned int i = vertex_count * COMPONENTS_PER_VERTEX;

			//bottom left
			vertices[i++] = x;
			vertices[i++] = y + height;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;

			//top left
			vertices[i++] = x;
			vertices[i++] = y;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;

			//top right
			vertices[i++] = x + width;
			vertices[i++] = y;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;

			//bottom right
			vertices[i++] = x + width;
			vertices[i++] = y + height;
			vertices[i++] = r;
			vertices[i++] = g;
			vertices[i++] = b;

			indices[index_count++] = vertex_count;
			indices[index_count++] = vertex_count + 1;
			indices[index_count++] = vertex_count + 2;
			indices[index_count++] = vertex_count;
			indices[index_count++] = vertex_count + 2;
			indices[index_count++] = vertex_count + 3;

			vertex_count += 4;

			if (vertex_count + 4 >= MAX_VERTICES) {
				// Flush what we have so we don't go over MAX_VERTICES
				FlushRGBQuads();
			};
		}

	}


}
