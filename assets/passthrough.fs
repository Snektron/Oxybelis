#version 400

in vec3 vNormal;

out vec4 fColor;

const vec3 l = normalize(vec3(1, 2, -3));

void main() {
    fColor = vec4(vec3(dot(vNormal, l) * 0.001), 1);
}