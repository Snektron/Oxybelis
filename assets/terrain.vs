#version 430

in vec3 aVertex;
in vec3 aNormal;

uniform mat4 uPerspective;
uniform mat4 uModel;

out vec3 vVertex;
out vec3 vNormal;
out vec4 vClipPos;

void main() {
    gl_Position = uPerspective * uModel * vec4(aVertex, 1.0);
    vVertex = aVertex;
    vNormal = aNormal;
    vClipPos = gl_Position;
}