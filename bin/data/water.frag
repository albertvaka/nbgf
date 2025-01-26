in mediump vec4 color;
in vec2 texCoord;

uniform sampler2D tex;
out vec4 fragColor;

uniform float time;
uniform vec2 resolution;

void main() {
    vec2 uv = texCoord.xy;

    float wave = sin(uv.y * 10.0 + time) * 0.0025;
    uv.x += wave;

    vec4 color = texture(tex, uv);

    fragColor = color;
}
