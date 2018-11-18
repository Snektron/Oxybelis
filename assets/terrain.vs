#version 430

in vec4 aVertex;
in vec4 aNormal;
in vec4 aColor;

uniform mat4 uPerspective;
uniform mat4 uModel;

out vec4 vClipPos;
out vec3 vNormal;
out vec3 vVertex;
out vec3 vColor;

void main() {
    gl_Position = uPerspective * uModel * aVertex;
    vClipPos = gl_Position;
    vNormal = aNormal.xyz;
    vVertex = aVertex.xyz;
    vColor = aColor.rgb;
}