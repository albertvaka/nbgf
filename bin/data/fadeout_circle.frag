in vec2 texCoord;
in vec2 vertex;

out vec4 fragColor;
uniform float progress; //0-1, 0 = full visible, 1 = full covered
uniform vec2 normalizedTarget; //coordinates x,y between 0,1 of the circle center
uniform float aspectRatio;

void main() {
    vec2 distanceFromTarget = texCoord - normalizedTarget;
    distanceFromTarget.x *= aspectRatio;
    float l = length(distanceFromTarget);

    if (l < progress) {
        discard;
    } else {
        fragColor = vec4(0,0,0,255);
    }
}
