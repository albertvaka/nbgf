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
		assertActive();
		GPU_SetUniformi(location, i);
		return *this;
	}

	Shader& SetUniform(int location, float f) {
		assertActive();
		GPU_SetUniformf(location, f);
		return *this;
	}

	Shader& SetUniform(int location, float x, float y) {
		assertActive();
		float v[] = { x,y };
		GPU_SetUniformfv(location, 2, 1, v);
		return *this;
	}

	Shader& SetUniform(int location, vec v) { return SetUniform(location, v.x, v.y); }

	Shader& SetUniform(int location, float r, float g, float b, float a) {
		assertActive();
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
