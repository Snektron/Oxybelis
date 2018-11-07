#version 430

in vec3 aVertex;
in vec3 aNormal;

uniform mat4 uPerspective;
uniform mat4 uModel;

out vec3 vVertex;
out vec3 vNormal;
out vec4 vClipPos;

const float PI = 3.14159264;
const float TAU = 2 * PI;
const float C = 0.001;
const float FAR = 10000000.0;

void main() {
    vec4 clipPos = uPerspective * uModel * vec4(aVertex, 1.0);
    gl_Position = clipPos;
    vClipPos = clipPos;

    vVertex = aVertex;
    vNormal = aNormal;
}