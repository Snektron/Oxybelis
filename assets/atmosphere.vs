#version 430

layout(location = 0) in vec2 aVertex;

out vec2 vFragCoord;
out vec3 vWorldPos;

uniform mat4 uProjection;
uniform mat4 uModel;

void main() {
    vec4 clip_pos = vec4(aVertex, 1, 1);
    gl_Position = vec4(aVertex, 0, 1);
    vFragCoord = aVertex / 2.0 + vec2(0.5);
    vec4 world_pos = inverse(uProjection * uModel) * clip_pos; 
    vWorldPos = world_pos.xyz;
}
