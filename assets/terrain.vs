#version 430

in vec3 aVertex;
in vec3 aNormal;
in vec3 aColor;

out vec4 vClipPos;
out vec3 vNormal;
out vec3 vVertex;
out vec3 vColor;

uniform mat4 uPerspective;
uniform mat4 uModel;

void main() {
    gl_Position = uPerspective * uModel * vec4(aVertex, 1);
    vClipPos = gl_Position;
    vNormal = aNormal;
    vVertex = aVertex;
    vColor = aColor;
}