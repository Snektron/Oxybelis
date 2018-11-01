#version 400

layout(location = 0) in vec3 aVertex;
layout(location = 1) in vec3 aNormal;

uniform mat4 uPerspective;
uniform mat4 uModel;

out vec3 vNormal;
out float vHeight;

const float PI = 3.14159264;
const float TAU = 2 * PI;
const float C = 0.001;
const float FAR = 100000000.0;

void main() {
    vec4 v = uPerspective * uModel * vec4(aVertex, 1.0);
    v.z = (2.0 * log(C * v.w + 1.0) / log(C * FAR + 1.0) - 1.0) * v.w;
    gl_Position = v;
    vNormal = aNormal;
    vHeight = (length(aVertex) - 6371000.0) / 10.0;
}