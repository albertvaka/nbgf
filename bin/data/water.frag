in mediump vec4 color;
in vec2 texCoord;

uniform sampler2D tex;
out vec4 fragColor;

uniform float time;
uniform vec2 resolution;

void main() {
    vec2 uv = texCoord.xy;

    // Simulate water distortion using a sine wave
    float wave = sin(uv.y * 10.0 + time) * 0.01;
    uv.x += wave;

    // Sample the texture with distortion
    vec4 color = texture2D(tex, uv);

    // Apply underwater tint (bluish)
    color.rgb *= vec3(0.5, 0.7, 1.0);

    // Simulate depth fog effect
    float depth = uv.y;
    color.rgb = mix(color.rgb, vec3(0.0, 0.2, 0.4), depth * 0.5);

    fragColor = color;
}
