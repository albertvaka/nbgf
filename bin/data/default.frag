in mediump vec4 color;
in vec2 texCoord;

uniform sampler2D tex;

out vec4 fragColor;

void main(void)
{
    fragColor = texture(tex, texCoord) * color;
}
