#version 430

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 21) out;

uniform mat4 uPerspective;
uniform mat4 uModel;

in vec3 vVertex[6];
in vec3 vNormal[6];
in vec4 vClipPos[6];

out vec3 gVertex;
out vec3 gNormal;
out vec4 gClipPos;
flat out int gSilhouette;

const vec3 LIGHT_DIR = normalize(vec3(1, 2, -3));
const float SHADOW_LENGTH = 10000;

vec4 project(vec3 v) {
    return uPerspective * uModel * vec4(v, 1.0);
}

void emit_shadow(int i, int j) {
    vec3 a = vVertex[i];
    vec3 b = vVertex[j];
    vec3 c = a + SHADOW_LENGTH * -LIGHT_DIR;
    vec3 d = b + SHADOW_LENGTH * -LIGHT_DIR;

    vec3 n1 = normalize(cross(b - a, c - a));
    vec3 n2 = normalize(cross(d - b, c - b));

    gl_Position = project(a);
    gVertex = a;
    gNormal = n1;
    gClipPos = gl_Position;
    gSilhouette = 1;
    EmitVertex();

    gl_Position = project(c);
    gVertex = c;
    gNormal = n1;
    gClipPos = gl_Position;
    gSilhouette = 1;
    EmitVertex();

    gl_Position = project(b);
    gVertex = b;
    gNormal = n1;
    gClipPos = gl_Position;
    gSilhouette = 1;
    EmitVertex();

    EndPrimitive();

    gl_Position = project(b);
    gVertex = b;
    gNormal = n2;
    gClipPos = gl_Position;
    gSilhouette = 1;
    EmitVertex();

    gl_Position = project(c);
    gVertex = c;
    gNormal = n2;
    gClipPos = gl_Position;
    gSilhouette = 1;
    EmitVertex();

    gl_Position = project(d);
    gVertex = d;
    gNormal = n2;
    gClipPos = gl_Position;
    gSilhouette = 1;
    EmitVertex();

    EndPrimitive();
}

void main() {
    vec3 normal = vNormal[0];

    int is_silhouette = 0;

    if (dot(normal, LIGHT_DIR) > 0) { // if this triangle is lit
        if (dot(vNormal[1], LIGHT_DIR) <= 0) {
            emit_shadow(0, 2);
        }

        if (dot(vNormal[3], LIGHT_DIR) <= 0) {
            emit_shadow(2, 4);
        }

        if (dot(vNormal[5], LIGHT_DIR) <= 0) {
            emit_shadow(4, 0);
        }

        // if (dot(vNormal[1], LIGHT_DIR) <= 0
        //     || dot(vNormal[3], LIGHT_DIR) <= 0
        //     || dot(vNormal[5], LIGHT_DIR) <= 0) // if any of the neighbors are unlit
        //     is_silhouette = 1;
    }

    for(int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i * 2].gl_Position;
        gVertex = vVertex[i * 2];
        gNormal = vNormal[i * 2];
        gClipPos = vClipPos[i * 2];
        gSilhouette = is_silhouette;
        EmitVertex();
    }

    EndPrimitive();
}
