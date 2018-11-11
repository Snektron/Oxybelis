#version 400

out vec4 fColor;

in vec2 vFragCoord;
in vec3 vRayDir;

uniform sampler2D uTerrain;
uniform sampler2D uNormalDistance;

uniform vec3 uCameraOrigin;
uniform vec3 uCameraDir;

const float PI = 3.14159265;

const vec2 MISS = vec2(1, -1);

const float RADIUS = 6371000;

const vec3 LIGHT_DIR = normalize(vec3(1, 2, -3));

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

void main() {
    vec3 terrain = texture(uTerrain, vFragCoord).rgb;
    vec4 normal_dist = texture(uNormalDistance, vFragCoord);

    vec3 rd = normalize(vRayDir);

    vec3 ground_radiance = vec3(0);
    float ground_alpha = 0;
    // Radiance reflected by ground

    if (normal_dist.a > 0) {
        vec3 p = uCameraOrigin + rd * normal_dist.a;
        vec3 n = normal_dist.xyz;

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

    fColor = vec4(pow(vec3(1) - exp(-radiance * 8.0 * 1e-5), vec3(1.0 / 2.2)), 1);
}
