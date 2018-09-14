#version 400

in vec3 vNormal;

out vec4 fColor;

void main() {
    fColor = vec4(abs(vNormal), 1);
}