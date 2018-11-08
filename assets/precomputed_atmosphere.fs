#version 400

out vec4 fColor;

in vec2 vFragCoord;
in vec3 vRayDir;

uniform sampler2D uTerrain;
uniform sampler2D uDistance;

uniform vec3 uCameraOrigin;
uniform vec3 uCameraUp;
uniform vec3 uCameraDir;

void main() {
    vec3 terrain = texture(uTerrain, vFragCoord).xyz;
    fColor = vec4(terrain, 1);
}
