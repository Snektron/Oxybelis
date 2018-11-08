#version 430

in vec3 vNormal;
in vec4 vClipPos;
in vec3 vVertex;

out vec4 fColor;
out float fDistance;

uniform vec3 uCameraOrigin;

const vec3 l = normalize(vec3(1, 2, -3));
const float C = 1.0;
const float FAR = 100000000.0;
const float OFFSET = 1.0;

void main() {
    vec3 ground = vec3(38. / 255., 89. / 255., 20. / 255.) * 0.8 * 0.8;

    fColor = vec4(max(dot(vNormal, l), 0) * ground, 1);
    fDistance = distance(uCameraOrigin, vVertex);
    gl_FragDepth = log(C * vClipPos.z + OFFSET) / log(C * FAR + OFFSET);
}