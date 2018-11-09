#version 400

out vec4 fColor;

in vec2 vFragCoord;
in vec3 vRayDir;

uniform sampler2D uTerrain;
uniform sampler2D uDistance;

uniform vec3 uCameraOrigin;
uniform vec3 uCameraDir;

const float PI = 3.14159265;

const vec2 MISS = vec2(1, -1);

const float RADIUS = 6371000;
const float ATMOS_RADIUS = RADIUS * 1.0094;

const vec3 LIGHT_DIR = normalize(vec3(1, 2, -3));

const vec3 GROUND_ALBEDO = vec3(38. / 255., 89. / 255., 20. / 255.) * 0.8;

#define USE_LUMINANCE

#ifdef USE_LUMINANCE
#define GetSolarRadiance GetSolarLuminance
#define GetSkyRadiance GetSkyLuminance
#define GetSkyRadianceToPoint GetSkyLuminanceToPoint
#define GetSunAndSkyIrradiance GetSunAndSkyIlluminance
#endif

vec3 GetSolarRadiance();
vec3 GetSkyRadiance(vec3 camera, vec3 view_ray, float shadow_length, vec3 sun_direction, out vec3 transmittance);
vec3 GetSkyRadianceToPoint(vec3 camera, vec3 point, float shadow_length, vec3 sun_direction, out vec3 transmittance);
vec3 GetSunAndSkyIrradiance(vec3 p, vec3 normal, vec3 sun_direction, out vec3 sky_irradiance);

vec2 ray_sphere_intersect(in vec3 ro, in vec3 rd, in float radius) {
    float b = dot(ro, rd);
    float c = dot(ro, ro) - radius * radius;

    float d = b * b - c;
    if (d < 0)
        return MISS;

    d = sqrt(d);
    return vec2(-b - d, -b + d);
}

void main() {
    vec3 terrain = texture(uTerrain, vFragCoord).xyz;
    float dist = texture(uDistance, vFragCoord).x;

    vec3 rd = normalize(vRayDir);
    float frag_angular_size = length(dFdx(rd) + dFdy(rd)) / length(rd);

    vec3 ground_radiance = vec3(0);
    float ground_alpha = 0;
    // Radiance reflected by ground

    if (dist > 0) {
        vec3 p = uCameraOrigin + rd * dist;
        vec3 n = normalize(p);

        vec3 sky_irradiance;
        vec3 sun_irradiance = GetSunAndSkyIrradiance(p, n, LIGHT_DIR, sky_irradiance);
        ground_radiance = terrain * (1.0 / PI) * (sun_irradiance + sky_irradiance);
        float shadow_length = 0;

        vec3 transmittance;
        vec3 in_scatter = GetSkyRadianceToPoint(uCameraOrigin, p, 0, LIGHT_DIR, transmittance);
        ground_radiance = ground_radiance * transmittance + in_scatter;
        ground_alpha = 1;
    }

    vec3 transmittance;
    vec3 radiance = GetSkyRadiance(uCameraOrigin, rd, 0, LIGHT_DIR, transmittance);

    radiance = mix(radiance, ground_radiance, ground_alpha);

    fColor = vec4(pow(vec3(1) - exp(-radiance * 10.0 * 1e-5), vec3(1.0 / 2.2)), 1);
}
