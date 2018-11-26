#version 430

layout(local_size_x = 32) in;

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

layout(binding = 2) uniform atomic_uint uTrianglePtr;

uniform uint uMaxOutputs;
uniform uint uNumVertices;
uniform vec3 uCenter;

const vec3 LIGHT_DIR = normalize(vec3(1, 2, -3));
const float SHADOW_LENGTH = 50000;
const float SHADOW_MIN_DST = 100000;
const vec3 SHADOW_OFFSET = SHADOW_LENGTH * -LIGHT_DIR;

ShadowVertex shadow_vertex(vec3 p, vec3 n) {
    return ShadowVertex(vec4(p, 1), vec4(n, 1));
}

void emit_shadow(vec3 a, vec3 b) {
    uint count = atomicCounterIncrement(uTrianglePtr);
    if (count >= uMaxOutputs)
        return;

    // address = count * <vertices in triangle> * <triangles in shadow>
    uint address = count * 3 * 2;

    vec3 c = a + SHADOW_OFFSET;
    vec3 d = b + SHADOW_OFFSET;

    vec3 normal = normalize(cross(b - a, c - a));

    shadow_vertices[address + 0] = shadow_vertex(a, normal);
    shadow_vertices[address + 1] = shadow_vertex(c, normal);
    shadow_vertices[address + 2] = shadow_vertex(b, normal);

    shadow_vertices[address + 3] = shadow_vertex(b, normal);
    shadow_vertices[address + 4] = shadow_vertex(c, normal);
    shadow_vertices[address + 5] = shadow_vertex(d, normal);
}

void emit_front_back(vec3 a, vec3 b, vec3 c, vec3 normal) {
    uint count = atomicCounterIncrement(uTrianglePtr);
    if (count >= uMaxOutputs)
        return;
    uint address = count * 3 * 2;

    vec3 d = a + SHADOW_OFFSET;
    vec3 e = b + SHADOW_OFFSET;
    vec3 f = c + SHADOW_OFFSET;

    vec3 inv_normal = -normal;

    shadow_vertices[address + 0] = shadow_vertex(a, inv_normal);
    shadow_vertices[address + 1] = shadow_vertex(c, inv_normal);
    shadow_vertices[address + 2] = shadow_vertex(b, inv_normal);

    shadow_vertices[address + 3] = shadow_vertex(d, normal);
    shadow_vertices[address + 4] = shadow_vertex(e, normal);
    shadow_vertices[address + 5] = shadow_vertex(f, normal);
}

void main() {
    uint id = gl_GlobalInvocationID.x;
    if (id >= uNumVertices)
        return;
    if (atomicCounter(uTrianglePtr) >= uMaxOutputs)
        return;

    uint index = id * 3;
    vec3 normal = vertices[index].normal.xyz;
    if (dot(normal, LIGHT_DIR) >= 0) // if this triangle is lit, skip it
        return;

    vec3 a = vertices[index + 0].position.xyz + uCenter;
    vec3 b = vertices[index + 1].position.xyz + uCenter;
    vec3 c = vertices[index + 2].position.xyz + uCenter;

    vec3 total = a + b + c;
    if (dot(total, total) >= SHADOW_MIN_DST * SHADOW_MIN_DST * 3)
        return;

    emit_front_back(a, b, c, normal);

    vec3 nd = vertices[index + 0].nn.xyz;
    if (dot(nd, LIGHT_DIR) >= 0) // if the neighbor is lit, emit a shadow
         emit_shadow(b, a);

    vec3 ne = vertices[index + 1].nn.xyz;
    if (dot(ne, LIGHT_DIR) >= 0)
         emit_shadow(c, b);

    vec3 nf = vertices[index + 2].nn.xyz;
    if (dot(nf, LIGHT_DIR) >= 0)
         emit_shadow(a, c);
}
