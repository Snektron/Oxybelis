#version 400

in vec3 aVertex;

uniform mat4 uPerspective;
uniform mat4 uModel;
uniform float uNumInstances;

out vec3 vNormal;

const float PI = 3.14159264;
const float TAU = 2 * PI;

void main() {
    gl_Position = uPerspective * uModel * vec4(aVertex, 1);
    vNormal = vec3(lessThan(aVertex, vec3(0)));
}