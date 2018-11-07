#version 430

layout(location = 0) in vec2 aVertex;

out vec2 vFragCoord;

uniform mat4 uProjection;

void main() {
    gl_Position = vec4(aVertex, 0, 1);
    vFragCoord = aVertex / 2.0 + vec2(0.5);
}
