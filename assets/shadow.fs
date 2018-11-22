#version 430

in vec3 vNormal;
in vec4 vClipPos;
in vec3 vVertex;

out vec2 dndz;
out vec2 zminmax;

uniform vec3 uCameraOrigin;
uniform sampler2D uNormalDistance;

const float C = 1.0;
const float FAR = 100000000.0;
const float OFFSET = 1.0;

void main() {
    dndz = vec2(1, 1);
    zminmax = vec2(1, 1);
    gl_FragDepth = log(C * vClipPos.z + OFFSET) / log(C * FAR + OFFSET);
}