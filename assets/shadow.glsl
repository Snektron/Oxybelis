#version 430

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

// Keep in sync with src/planet/terragen/TerrainData TerrainData::VertexData
struct Vertex {
    vec4 position;
    vec4 normal;
    vec4 color;
    vec4 nn;
};

struct ShadowVertex {
    vec4 position;
    vec4 normal;
};

layout(std430, binding = 0) buffer terrain {
    Vertex vertices[];
};

layout(std430, binding = 1) buffer shadow {
    ShadowVertex shadow_vertices[];
};

layout(binding = 2) uniform atomic_uint uElementsWritten;

uniform uint uMaxOutputs;
uniform uint uNumVertices;
uniform vec3 uCenter;
uniform vec3 uCameraOrigin;

const vec3 LIGHT_DIR = normalize(vec3(1, 2, -3));
const float SHADOW_LENGTH = 50000;
const float SHADOW_MIN_DST = 100000;

ShadowVertex shadow_vertex(vec3 p, vec3 n) {
    return ShadowVertex(vec4(p, 1), vec4(n, 1));
}

void emit_shadow(vec3 a, vec3 b) {
    uint index = atomicCounterIncrement(uElementsWritten);
    if (index >= uMaxOutputs)
        return;

    // address = index * <vertices in triangle> * <triangles in shadow>
    uint address = index * 3 * 2;

    vec3 c = a + SHADOW_LENGTH * -LIGHT_DIR;
    vec3 d = b + SHADOW_LENGTH * -LIGHT_DIR;

    vec3 n1 = normalize(cross(b - a, c - a));
    vec3 n2 = normalize(cross(d - b, c - b));

    shadow_vertices[address + 0] = shadow_vertex(a, n1);
    shadow_vertices[address + 1] = shadow_vertex(c, n1);
    shadow_vertices[address + 2] = shadow_vertex(b, n1);

    shadow_vertices[address + 3] = shadow_vertex(b, n2);
    shadow_vertices[address + 4] = shadow_vertex(c, n2);
    shadow_vertices[address + 5] = shadow_vertex(d, n2);
}

void main() {
    uint id = gl_GlobalInvocationID.x;
    if (id >= uNumVertices)
        return;

    uint index = id * 3;
    vec3 normal = vertices[index].normal.xyz;
    vec3 a = vertices[index + 0].position.xyz + uCenter;
    vec3 b = vertices[index + 1].position.xyz + uCenter;
    vec3 c = vertices[index + 2].position.xyz + uCenter;

    if (distance((a + b + c) / 3, uCameraOrigin) > SHADOW_MIN_DST)
        return;

    vec3 nd = vertices[index + 0].nn.xyz;
    vec3 ne = vertices[index + 1].nn.xyz;
    vec3 nf = vertices[index + 2].nn.xyz;

    if (dot(normal, LIGHT_DIR) > 0) { // if this triangle is lit
        if (dot(nd, LIGHT_DIR) < 0)
            emit_shadow(a, b);

        if (dot(ne, LIGHT_DIR) < 0)
            emit_shadow(b, c);

        if (dot(nf, LIGHT_DIR) < 0)
            emit_shadow(c, a);
    }
}
