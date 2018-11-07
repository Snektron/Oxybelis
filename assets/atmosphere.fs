#version 430

out vec4 fColor;

in vec2 vFragCoord;

uniform sampler2D uTerrain;

void main() {
    fColor = texture(uTerrain, vFragCoord);
}
