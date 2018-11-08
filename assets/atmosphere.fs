#version 430

out vec4 fColor;

in vec2 vFragCoord;
in vec3 vWorldPos;

uniform sampler2D uTerrain;
uniform vec3 uRayOrigin;

void main() {
    // fColor = texture(uTerrain, vFragCoord);

    vec3 ray = normalize(vWorldPos - uRayOrigin);

    fColor = vec4(ray, 1);
}
