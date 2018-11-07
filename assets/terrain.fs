#version 430

in vec3 gNormal;
in vec4 gClipPos;

out vec4 fColor;

const vec3 l = normalize(vec3(1, 2, -3));
const float C = 1.0;
const float FAR = 100000000.0;
const float OFFSET = 1.0;

void main() {
    vec3 ground = vec3(38. / 255., 89. / 255., 20. / 255.) * 0.8 * 0.8;

    fColor = vec4(vec3(dot(gNormal, l)) * ground + 0.2, 1);
    gl_FragDepth = log(C * gClipPos.z + OFFSET) / log(C * FAR + OFFSET);
}