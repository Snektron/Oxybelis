#version 430

in vec3 vNormal;
in vec4 vClipPos;
in vec3 vVertex;
in vec2 vFragCoord;

out vec2 dndz;
out vec2 zminmax;

uniform vec3 uCameraOrigin;
uniform vec3 uCameraDir;
layout(binding = 1) uniform sampler2D uNormalDistance;

const float C = 1.0;
const float FAR = 100000000.0;
const float OFFSET = 1.0;

void main() {
    float terrain_d = texelFetch(uNormalDistance, ivec2(gl_FragCoord.xy), 0).a;
    float d = distance(uCameraOrigin, vVertex);

    if (d >= terrain_d)
        discard;

    if (gl_FrontFacing) {
        dndz = vec2(-1, -d); // decrement delta n, z
        zminmax = vec2(-d, d);
    } else {
        dndz = vec2(1, d); // increment delta n, z
        zminmax = vec2(-d, d);
    }

    gl_FragDepth = log(C * vClipPos.z + OFFSET) / log(C * FAR + OFFSET);
}