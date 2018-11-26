#version 430

in vec3 vNormal;
in vec4 vClipPos;
in vec3 vVertex;
in vec2 vFragCoord;

out vec2 dndz;
out vec2 zminmax;

uniform sampler2D uNormalDistance;

void main() {
    float terrain_d = texelFetch(uNormalDistance, ivec2(gl_FragCoord.xy), 0).a;
    float d = length(vVertex);

    if (terrain_d > 0 && round(d) >= round(terrain_d))
        discard;

    if (gl_FrontFacing) {
        dndz = vec2(-1, -d); // decrement delta n, z
    } else {
        dndz = vec2(1, d); // increment delta n, z
    }

    zminmax = vec2(-d, d);
}