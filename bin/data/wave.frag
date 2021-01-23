// Based in https://github.com/Gonkee/Gonkees-Shaders/blob/master/water.shader

in mediump vec4 color;
in vec2 texCoord;
in vec2 vertex;

uniform sampler2D tex;
uniform float time;
float scale = 2.f;
float intensity = 1.5f/6.f;

out vec4 fragColor;

float rand(vec2 coord){
	return fract(sin(dot(coord, vec2(12.9898, 78.233)))* 43758.5453123);
}

float noise(vec2 coord){
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

void main(){

	vec2 noisecoord1 = vertex * 0.01 * scale;
	vec2 noisecoord2 = vertex * 0.01 * scale + 4.0;

	vec2 motion1 = vec2(time * 0.3, time * -0.4);
	vec2 motion2 = vec2(time * 0.1, time * 0.5);

	vec2 distort1 = vec2(noise(noisecoord1 + motion1), noise(noisecoord2 + motion1)) - vec2(0.5);
	vec2 distort2 = vec2(noise(noisecoord1 + motion2), noise(noisecoord2 + motion2)) - vec2(0.5);

	vec2 distort_sum = (distort1 + distort2) * intensity * 0.01;

	fragColor = texture(tex, texCoord + distort_sum) * color;
}
