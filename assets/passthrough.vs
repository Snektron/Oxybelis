#version 400

in vec3 aVertex;
in vec3 aNormal;

uniform mat4 uMvp;

out vec3 vNormal;

void main() {
    gl_Position = uMvp * vec4(aVertex, 1);
    vNormal = vec3(lessThan(aVertex, vec3(0)));
}