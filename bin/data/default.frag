in mediump vec4 color;
in vec2 texCoord;

uniform sampler2D tex;

out vec4 fragColor;

void main(void)
{
    fragColor = vec4(0.f, 1.0f, 0.f, 0.f);
}
