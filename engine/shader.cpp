#include "shader.h"

#include "debug.h"

#include <SDL_gpu.h>

void Shader::loadAndAttach(GPU_ShaderEnum type, const char* path) {

	std::stringstream source;
	GPU_Renderer* renderer = GPU_GetCurrentRenderer();
	if (renderer->shader_language == GPU_LANGUAGE_GLSL)
	{
		if (renderer->max_shader_version >= 330) {
			source << "#version 330\n";
		} else {
			source << "#version 130\n";
		}
	}
	else if (renderer->shader_language == GPU_LANGUAGE_GLSLES)
	{
		if (renderer->max_shader_version >= 300) {
			source << "#version 300 es\n";
		} else {
			source << "#version 200 es\n";
		}

		if (type == GPU_FRAGMENT_SHADER) {
			source << "#ifdef GL_FRAGMENT_PRECISION_HIGH\n\
precision highp float;\n\
#else\n\
precision mediump float;\n\
#endif\n\
precision mediump int;\n";
		} else {
			source << "precision highp float;\nprecision mediump int;\n";
		}
	}

	source << std::ifstream(path).rdbuf();

	int id = GPU_CompileShader(type, source.str().c_str());
	GPU_AttachShader(program, id);
}

void Shader::Load(const char* vertex_path, const char* geometry_path, const char* fragment_path) {
	program = GPU_CreateShaderProgram();
	if (vertex_path) {
		loadAndAttach(GPU_ShaderEnum::GPU_VERTEX_SHADER, vertex_path);
	}
	if (geometry_path) {
		loadAndAttach(GPU_ShaderEnum::GPU_GEOMETRY_SHADER, geometry_path);
	}
	if (fragment_path) {
		loadAndAttach(GPU_ShaderEnum::GPU_FRAGMENT_SHADER, fragment_path);
	}
	GPU_LinkShaderProgram(program);
	block = GPU_LoadShaderBlock(program, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix");
}

int Shader::GetUniformLocation(const char* name) {
	std::map<std::string, int>::const_iterator it = uniforms.find(name);
	if (it != uniforms.end()) {
		return it->second;
	}
	else {
		int location = GPU_GetUniformLocation(program, name);
		uniforms.insert(std::make_pair(name, location));
		if (location == -1) {
			Debug::out << "Uniform \"" << name << "\" not found in shader";
		}

		return location;
	}
}
