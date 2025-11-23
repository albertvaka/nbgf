// enable depth test with GPU_SetDepthTest to use this shader properly

in vec2 gpu_Vertex;
uniform float z;
in vec2 gpu_TexCoord;
in vec4 gpu_Color;
uniform mat4 gpu_ModelViewProjectionMatrix;

out vec4 color;
out vec2 texCoord;

void main(void)
{
	color = gpu_Color;
	texCoord = vec2(gpu_TexCoord);
	gl_Position = gpu_ModelViewProjectionMatrix * vec4(gpu_Vertex.x, gpu_Vertex.y, z, 1.0);
}
