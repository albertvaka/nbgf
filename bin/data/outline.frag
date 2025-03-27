out vec4 fragColor;

in vec2 vertex;
in vec2 texCoord;

uniform sampler2D tex;
uniform vec2 offset;
uniform vec2 iResolution;
uniform float iTime;
uniform float zoom;
uniform float windowScale;

const float alphaThreshold = 0.5;

#define FOAM_COL vec4(0.7, 0.8, 0.9, 1.0)

// Perlin noise by @XorDev
// https://mini.gmshaders.com/p/gm-shaders-mini-noise-1437243

//vec2 version of the hash function
vec2 hash2(vec2 p)
{
    //Generate a pseudo random vec2 from 'p'
    return fract(sin(p * mat2(0.129898, 0.81314, 0.78233,  0.15926)) * 43758.5453);
}
//vec2 unit-vector version of the hash function
vec2 hash2_norm(vec2 p)
{
    //Returns a random normalized direction vector
    return normalize(hash2(p) - 0.5);
}
//Standard Perlin noise
float perlin_noise(vec2 p)
{
    //Cell (whole number) coordinates
    vec2 cell = floor(p);
    //Sub-cell (fractional) coordinates
    vec2 sub = p - cell;
    //Quintic interpolation
    vec2 quint = sub*sub*sub*(10.0 + sub*(-15.0 + 6.0*sub));
    //Offset vector
    const vec2 off = vec2(0,1);

    //Compute corner hashes and gradients
    float grad_corner00 = dot(hash2_norm(cell+off.xx), off.xx-sub);
    float grad_corner10 = dot(hash2_norm(cell+off.yx), off.yx-sub);
    float grad_corner01 = dot(hash2_norm(cell+off.xy), off.xy-sub);
    float grad_corner11 = dot(hash2_norm(cell+off.yy), off.yy-sub);

    //Interpolate between the gradient values them and map to 0 - 1 range
    return mix(mix(grad_corner00, grad_corner10, quint.x),
               mix(grad_corner01, grad_corner11, quint.x), quint.y) * 0.7 + 0.5;
}

void main()
{
    vec2 coord = vertex * iResolution + offset*zoom;
    coord /= 250000.0 * windowScale;

    // Those noise functions could be replaced by sampling a texture if this is too slow
    float wobble = perlin_noise(coord*10.0 + vec2(iTime*0.4, iTime*0.55)); // make the whole image wobble
    float wobleAmount = 0.008*zoom;

	//fragColor = vec4(wobble,wobble,wobble,255);
    //return;

    vec4 neighbor_color = texture(tex, texCoord + (0.5 - wobble) * wobleAmount);

    bool isFoam = (neighbor_color.a > alphaThreshold);

    if (isFoam) {
        fragColor = FOAM_COL;
    } else {
        fragColor = vec4(0,0,0,0);
    }
}
