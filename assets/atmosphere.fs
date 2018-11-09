#version 430

out vec4 fColor;

in vec2 vFragCoord;
in vec3 vRayDir;

uniform sampler2D uTerrain;
uniform sampler2D uNormalDistance;

uniform vec3 uCameraOrigin;
uniform vec3 uCameraDir;

const float PI = 3.14159265;

const vec2 MISS = vec2(1, -1);

const float RADIUS = 6371000.0;

const float ATMOS_RADIUS = RADIUS * 1.025;
const float ATMOS_HEIGHT = ATMOS_RADIUS - RADIUS;
const vec3 ATMOS_COLOR = vec3(0.3, 0.7, 1.0);
const float ATMOS_AVG_DENSITY_HEIGHT = 0.25;

const vec3 LIGHT_DIR = normalize(vec3(1, 2, -3));

const float MIE_G = -0.85;
const float MIE_G2 = MIE_G * MIE_G;

const int IN_SCATTER_STEPS = 10;
const int OUT_SCATTER_STEPS = 10;

const float K_R = 0.166;
const float K_M = 0.025;
const float INTENSITY = 14.3;

vec2 ray_sphere_intersect(in vec3 ro, in vec3 rd, in float radius) {
    float b = dot(ro, rd);
    float c = dot(ro, ro) - radius * radius;

    float d = b * b - c;
    if (d < 0)
        return MISS;

    d = sqrt(d);
    return vec2(-b - d, -b + d);
}

float phase_mie(in float x) {
    float top = (1 - MIE_G2) * (1 + x * x);
    float bot = 1 + MIE_G2 - 2 * MIE_G * x;
    bot *= sqrt(bot);
    bot *= 2 + MIE_G2;
    return 1.5 * top / bot;
}

float phase_rayleigh(in float x) {
    return 0.75 * (1 + x * x);
}

float density(in vec3 p) {
    float h = (length(p) - RADIUS) / ATMOS_HEIGHT;
    return exp(-h / ATMOS_AVG_DENSITY_HEIGHT);
}

float out_scatter(vec3 pa, vec3 pb) {
    vec3 rs = (pb - pa) / float(OUT_SCATTER_STEPS);
    vec3 p = pa + rs * 0.5;

    float total = 0;
    for (int i = 0; i < OUT_SCATTER_STEPS; ++i) {
        total += density(p);
        p += rs;
    }

    return total * length(rs) / ATMOS_HEIGHT;
}

vec3 in_scatter(in vec3 ro, in vec3 rd, vec2 t) {
    float segment_length = (t.y - t.x) / float(IN_SCATTER_STEPS);
    vec3 rs = rd * segment_length;
    vec3 pa = ro + rd * t.x;
    vec3 pb = ro + rd * t.y;

    vec3 p = pa + 0.5 * rs;

    vec3 total = vec3(0);
    for (int i = 0; i < IN_SCATTER_STEPS; ++i) {
        vec2 d = ray_sphere_intersect(p, LIGHT_DIR, ATMOS_RADIUS);
        vec3 pc = p + LIGHT_DIR * d.y; // atmosphere intersection point
        float x = -(out_scatter(pa, p) + out_scatter(p, pc)) * PI * 4;
        total += density(p) * exp(x * (K_R * ATMOS_COLOR + K_M));
        p += rs;
    }

    total *= segment_length / ATMOS_HEIGHT;
    // return total;
    float theta = dot(rd, -LIGHT_DIR);
    return total * INTENSITY * (K_R * ATMOS_COLOR * phase_rayleigh(theta) + K_M * phase_mie(theta));
}

vec3 scatter(in vec3 ro, in vec3 rd) {
    vec2 e = ray_sphere_intersect(ro, rd, ATMOS_RADIUS);
    if (e.y < 0)
        return vec3(0); // ray misses atmosphere

    e.x = max(e.x, 0);

    float dist = texture(uNormalDistance, vFragCoord).a;
    if (dist > 0)
        e.y = min(e.y, dist);

    return in_scatter(ro, rd, e);
}

void main() {
    vec3 terrain = texture(uTerrain, vFragCoord).xyz;
    vec3 atmos_color = scatter(uCameraOrigin, normalize(vRayDir));
    const float exposure = 0.5;
    atmos_color = 1.0 - exp(-exposure * atmos_color);

    fColor = vec4(terrain + atmos_color, 1);
}
