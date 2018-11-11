#version 430

in vec3 gVertex;
in vec3 gNormal;
in vec4 gClipPos;
flat in int gSilhouette;

out vec4 fColor;
out vec4 fNormalDistance;

uniform vec3 uCameraOrigin;

const vec3 l = normalize(vec3(1, 2, -3));
const float C = 1.0;
const float FAR = 100000000.0;
const float OFFSET = 1.0;

const vec3 GROUND_ALBEDO = vec3(38. / 255., 89. / 255., 20. / 255.) * 0.2;
const vec3 GROUND_SILHOUETTE_ALBEDO = vec3(1, 0, 1);

void main() {
    if (gSilhouette == 1)
        fColor = vec4(GROUND_SILHOUETTE_ALBEDO, 1);
    else
        fColor = vec4(GROUND_ALBEDO, 1);

    float d = distance(uCameraOrigin, gVertex);
    fNormalDistance = vec4(gNormal, d);

    gl_FragDepth = log(C * gClipPos.z + OFFSET) / log(C * FAR + OFFSET);
}
