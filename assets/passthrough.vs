#version 400

in vec3 aVertex;
in vec3 aNormal;

uniform mat4 uPerspective;
uniform mat4 uModel;
uniform float uNumInstances;

out vec3 vNormal;

const float PI = 3.14159264;
const float TAU = 2 * PI;

void main() {
    float a = float(gl_InstanceID) / uNumInstances * TAU;
    gl_Position = uPerspective * (vec4(cos(a) * 3, sin(a) * 3, 0, 0) + uModel * vec4(aVertex * vec3(.1), 1));
    vNormal = vec3(lessThan(aVertex, vec3(0)));
}