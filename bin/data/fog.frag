// Based in https://github.com/Gonkee/Gonkees-Shaders/blob/master/fog.shader

in mediump vec4 color;
in vec4 gl_FragCoord;
in vec2 vertex;

uniform vec2 offset = vec2(0,0);
uniform float time = 0;
uniform int octaves = 4;
uniform vec4 fogColor = vec4(0.7, 0.5, 0.7, 0.5);

out vec4 fragColor;

float rand(vec2 coord) {
	return fract(sin(dot(coord, vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 coord) {
	vec2 i = floor(coord);
	vec2 f = fract(coord);

	// 4 corners of a rectangle surrounding our point
	float a = rand(i);
	float b = rand(i + vec2(1.0, 0.0));
	float c = rand(i + vec2(0.0, 1.0));
	float d = rand(i + vec2(1.0, 1.0));

	vec2 cubic = f * f * (3.0 - 2.0 * f);

	return mix(a, b, cubic.x) + (c - a) * cubic.y * (1.0 - cubic.x) + (d - b) * cubic.x * cubic.y;
}

float fbm(vec2 coord) {
	float value = 0.0;
	float scale = 0.5;
	for(int i = 0; i < octaves; i++){
		value += noise(coord) * scale;
		coord *= 2.0;
		scale *= 0.5;
	}
	return value;
}

void main() {

	vec2 coord = vec2(vertex.x, vertex.y);

    coord /= 60.f;
    coord += offset;

	vec2 motion = vec2( fbm(coord + vec2(time * -0.3, time * 0.3)) );
	float final = fbm(coord + motion);

	fragColor = vec4(fogColor.r, fogColor.g, fogColor.b, final*0.7-0.2);
}



