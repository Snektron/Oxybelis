#version 400

in vec3 aVertex;
in vec3 aNormal;

uniform mat4 uPerspective;
uniform mat4 uModel;

out vec3 vNormal;

const float PI = 3.14159264;
const float TAU = 2 * PI;

void main() {
    gl_Position = uPerspective * uModel * vec4(aVertex, 1);
    vNormal = aNormal;
}