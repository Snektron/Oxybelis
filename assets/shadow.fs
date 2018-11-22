#version 430

in vec3 vNormal;
in vec4 vClipPos;
in vec3 vVertex;

out vec4 fColor;
out vec4 fNormalDistance;

uniform vec3 uCameraOrigin;

const float C = 1.0;
const float FAR = 100000000.0;
const float OFFSET = 1.0;

void main() {
    fColor = vec4(1, 0, 1, 1);
    float d = distance(uCameraOrigin, vVertex);
    fNormalDistance = vec4(vNormal, d);
    gl_FragDepth = log(C * vClipPos.z + OFFSET) / log(C * FAR + OFFSET);
}