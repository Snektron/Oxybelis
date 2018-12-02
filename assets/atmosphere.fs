#version 400

out vec4 fColor;

in vec2 vFragCoord;
in vec3 vRayDir;

uniform sampler2D uTerrain;
uniform sampler2D uNormalDistance;
uniform sampler2D uDndz;
uniform sampler2D uZminmax;
uniform samplerCube uSkybox;

uniform vec3 uCameraOrigin;
uniform vec3 uCameraDir;

const float PI = 3.141592654;

const vec3 LIGHT_DIR = normalize(vec3(1, 2, -3));
const float EXPOSURE = 1e-4;

vec3 GetSolarLuminance();
vec3 GetSkyLuminance(vec3 camera, vec3 view_ray, float shadow_length, vec3 sun_direction, out vec3 transmittance);
vec3 GetSkyLuminanceToPoint(vec3 camera, vec3 point, float shadow_length, vec3 sun_direction, out vec3 transmittance);
vec3 GetSunAndSkyIlluminance(vec3 p, vec3 normal, vec3 sun_direction, out vec3 sky_irradiance);

float clamp2(float x, float minv, float maxv) {
    return max(min(x, maxv), minv);
}

void main() {
    vec3 rd = normalize(vRayDir);

    vec3 terrain = texture(uTerrain, vFragCoord).rgb;
    vec4 normal_dist = texture(uNormalDistance, vFragCoord);
    vec2 dndz = texture(uDndz, vFragCoord).rg;
    vec2 zminmax = texture(uZminmax, vFragCoord).rg;
    vec3 space = texture(uSkybox, rd).xyz;

    zminmax.r *= -1;
    space = log(1.0001 - space) / -(EXPOSURE * 60.0);

    vec3 ground_radiance = vec3(0);
    float ground_alpha = 0;

    if (normal_dist.a > 0) {
        vec3 p = uCameraOrigin + rd * normal_dist.a;
        vec3 n = normal_dist.xyz;

        float shadow_length = clamp2(dndz.g - dndz.r * normal_dist.a, 0, normal_dist.a - zminmax.r);

        float sun_visibility = 1.0;
        if (dndz.r < -0.1)
            sun_visibility = 1.0 - smoothstep(0, 10, shadow_length);

        vec3 sky_irradiance;
        vec3 sun_irradiance = GetSunAndSkyIlluminance(p, n, LIGHT_DIR, sky_irradiance);
        ground_radiance = terrain * (1.0 / PI) * (sun_irradiance * sun_visibility + sky_irradiance);

        vec3 transmittance;
        vec3 in_scatter = GetSkyLuminanceToPoint(uCameraOrigin, p, shadow_length, LIGHT_DIR, transmittance);
        ground_radiance = ground_radiance * transmittance + in_scatter;
        ground_alpha = 1;
    }

    float shadow_length = clamp2(dndz.g, 0, zminmax.g);
    vec3 transmittance;
    vec3 radiance = GetSkyLuminance(uCameraOrigin, rd, shadow_length, LIGHT_DIR, transmittance);

    radiance += max(transmittance * GetSolarLuminance() * pow(dot(rd, LIGHT_DIR) - 0.003, 3000), 0);

    vec3 space_alpha = clamp(vec3(1) - (vec3(1) - transmittance), vec3(0), vec3(1));
    radiance += space * space_alpha;

    radiance = mix(radiance, ground_radiance, ground_alpha);

    vec3 color = pow(vec3(1) - exp(-radiance * EXPOSURE), vec3(1.0 / 2.2));

    fColor = vec4(color.xyz, 1);
}
