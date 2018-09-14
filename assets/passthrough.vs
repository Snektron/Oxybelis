#version 400

in vec2 aVertex;

uniform mat4 uMvp;

void main() {
    gl_Position = uMvp * vec4(aVertex, -10, 1);
}