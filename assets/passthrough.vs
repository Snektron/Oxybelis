#version 400

layout(location = 0) in vec3 aVertex;
layout(location = 1) in vec3 aNormal;

uniform mat4 uPerspective;
uniform mat4 uModel;

out vec3 vNormal;
out float vHeight;

const float PI = 3.14159264;
const float TAU = 2 * PI;

void main() {
    gl_Position = uPerspective * uModel * vec4(aVertex, 1);
    vNormal = aNormal;
    vHeight = aVertex.y;
}