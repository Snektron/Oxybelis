#version 430

layout(location = 0) in vec2 aVertex;

out vec2 vFragCoord;
out vec3 vRayDir;

uniform mat4 uInvProjection;
uniform mat4 uModel;

uniform vec3 uCameraOrigin;
uniform vec3 uCameraUp;
uniform vec3 uCameraDir;

void main() {
    vec4 clip = vec4(aVertex, 0, 1);
    gl_Position = clip;
    vFragCoord = aVertex / 2.0 + vec2(0.5);

    clip = uInvProjection * clip;
    clip.w = 0;
    clip = inverse(uModel) * clip;
    vRayDir = vec3(clip);
}
