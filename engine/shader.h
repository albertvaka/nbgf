#pragma once

#include <map>
#include <string>

#include "vec.h"
#include "debug.h"
#include "SDL_gpu.h"

struct Shader {
	// Pass in nullptrs to use the defaults
	void Load(const char* vertex_path, const char* geometry_path, const char* fragment_path);

	Shader& SetUniform(int location, int i) {
#ifdef _DEBUG
		assertActive();
#endif
		GPU_SetUniformi(location, i);
		return *this;
	}

	Shader& SetUniform(int location, float f) {
#ifdef _DEBUG
		assertActive();
#endif
		GPU_SetUniformf(location, f);
		return *this;
	}

	Shader& SetUniform(int location, float x, float y) {
#ifdef _DEBUG
		assertActive();
#endif
		float v[] = { x,y };
		GPU_SetUniformfv(location, 2, 1, v);
		return *this;
	}

	Shader& SetUniform(int location, vec v) { return SetUniform(location, v.x, v.y); }

	Shader& SetUniform(int location, float r, float g, float b, float a) {
#ifdef _DEBUG
		assertActive();
#endif
		float v[] = { r,g,b,a };
		GPU_SetUniformfv(location, 4, 1, v);
		return *this;
	}

	Shader& SetUniform(const char* name, int i) { return SetUniform(GetUniformLocation(name), i); }
	Shader& SetUniform(const char* name, float f) { return SetUniform(GetUniformLocation(name), f); }
	Shader& SetUniform(const char* name, float x, float y) { return SetUniform(GetUniformLocation(name), x, y); }
	Shader& SetUniform(const char* name, vec v) { return SetUniform(GetUniformLocation(name), v); }
	Shader& SetUniform(const char* name, float r, float g, float b, float a) { return SetUniform(GetUniformLocation(name), r, g, b, a); }

	Shader& SetTexture(const char* textureSamplerUniformName, GPU_Image* image, int location = 1) { 
		GPU_SetShaderImage(image, GetUniformLocation(textureSamplerUniformName), location); 
		return *this;
	}

	void Activate() {
		if (GPU_GetCurrentShaderProgram() != program) {
			GPU_ActivateShaderProgram(program, &block);
		}
	}
	static void Deactivate() { GPU_DeactivateShaderProgram(); }

	int GetUniformLocation(const char* name, bool warnIfNotFound = true);

	unsigned int program = -1;
	GPU_ShaderBlock block;
	std::string shaderFilePaths;

private:
	void loadAndAttach(GPU_ShaderEnum type, const char* path);
	void assertActive() {
		if (GPU_GetCurrentShaderProgram() != program) {
			Debug::out << "Can't set uniform on inactive shader";
		}
	}
	std::map<std::string, int> uniforms;
};

struct IntCachedUniform {
	int location;
	int cached = 1234567;
	IntCachedUniform() = default;
	IntCachedUniform(int location) : location(location) {}
	void Set(int value) {
		if (value != cached) {
			cached = value;
			GPU_SetUniformi(location, cached);
		}
	}
};

struct FloatCachedUniform {
	int location;
	float cached = 1234567.f;
	FloatCachedUniform() = default;
	FloatCachedUniform(int location) : location(location) {}
	void Set(float value) {
		if (value != cached) {
			cached = value;
			GPU_SetUniformf(location, cached);
		}
	}
};

struct Float2CachedUniform {
	int location;
	float cached[2] = { 1234567.f, -1234567.f };
	Float2CachedUniform() = default;
	Float2CachedUniform(int location) : location(location) {}
	void Set(float x, float y) {
		if (x != cached[0] || y != cached[1]) {
			cached[0] = x;
			cached[1] = y;
			GPU_SetUniformfv(location, 2, 1, cached);
		}
	}
	void Set(vec v) { Set(v.x, v.y); }
};

struct Float4CachedUniform {
	int location;
	float cached[4] = { 1234567.f, -1234567.f, 1234567.f, -1234567.f };
	Float4CachedUniform() = default;
	Float4CachedUniform(int location) : location(location) {}
	void Set(float r, float g, float b, float a) {
		if (r != cached[0] || g != cached[1] || b != cached[2] || a != cached[3]) {
			cached[0] = r;
			cached[1] = g;
			cached[2] = b;
			cached[3] = a;
			GPU_SetUniformfv(location, 4, 1, cached);
		}
	}
};
