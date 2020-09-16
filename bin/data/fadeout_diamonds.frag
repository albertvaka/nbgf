//Code from https://ddrkirby.com/articles/shader-based-transitions/shader-based-transitions.html

in vec2 texCoord;
in vec2 vertex;

out vec4 fragColor;
uniform float fadeOutProgress = 0; //0-1

float diamondPixelSize = 20;

void main() {
    float xFraction = fract(vertex.x / diamondPixelSize);
    float yFraction = fract(vertex.y / diamondPixelSize);
    
    float xDistance = abs(xFraction - 0.5);
    float yDistance = abs(yFraction - 0.5);
    
    float val =  xDistance + yDistance + texCoord.x + texCoord.y;

    if (val <= fadeOutProgress * 3) {
        discard;
    } else {
        fragColor = vec4(0,0,0,255);
    }
}
