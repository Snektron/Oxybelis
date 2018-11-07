#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

uniform mat4 uPerspective;
uniform mat4 uModel;

in vec3 vVertex[3];
in vec3 vNormal[3];
in vec4 vClipPos[3];

out vec3 gNormal;
out vec4 gClipPos;

const vec3 l = normalize(vec3(1, 2, -3));

void main() {
    for(int i = 0; i < gl_in.length(); ++i) {
        gl_Position = gl_in[i].gl_Position;
        gNormal = vNormal[i];
        gClipPos = vClipPos[i];
        EmitVertex();
    }

    EndPrimitive();

    for(int i = 0; i < gl_in.length(); ++i) {
        gl_Position = gl_in[i].gl_Position;
        gl_Position.z = 1;
        gNormal = vNormal[i];
        gClipPos = vClipPos[i];
        EmitVertex();
    }

    EndPrimitive();
}
