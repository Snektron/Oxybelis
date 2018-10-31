#version 400

in vec3 vNormal;
in float vHeight;

out vec4 fColor;

const vec3 l = normalize(vec3(1, 2, -3));

void main() {
    float x = smoothstep(0, 1, vHeight) * 2.;

    vec3 low = vec3(38. / 255., 89. / 255., 20. / 255.) * 0.8;
    vec3 hi = vec3(0.7);

    // fColor = vec4(vec3(dot(vNormal, l)), 1);
    // fColor = vec4(vNormal, 1);
    fColor = vec4(vec3(dot(vNormal, l)) * mix(low, hi, x) * 0.8 + 0.2, 1);
}