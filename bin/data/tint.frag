in mediump vec4 color;
in vec2 texCoord;

uniform sampler2D tex;
uniform vec4 flashColor;

out vec4 fragColor;

void main()
{
	vec4 pixel_color = texture(tex, texCoord);
	float percent = flashColor.a;

	vec4 colorDifference = vec4(0,0,0,1);

	colorDifference.r = flashColor.r - pixel_color.r;
	colorDifference.g = flashColor.g - pixel_color.g;
	colorDifference.b = flashColor.b - pixel_color.b;
	pixel_color.r = pixel_color.r + colorDifference.r * percent;
	pixel_color.g = pixel_color.g +colorDifference.g * percent;
	pixel_color.b = pixel_color.b + colorDifference.b * percent;

	fragColor = pixel_color;
}
