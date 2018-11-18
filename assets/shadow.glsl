#version 430

layout(local_size_x = 16, local_size_y = 1, local_size_z = 1) in;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec3 color;
};

layout(packed, binding = 0) buffer terrain {
    Vertex vertices[];
};

layout(packed, binding = 1) buffer shadow {
    vec4 shadow_vertices[];
};

layout(binding = 2) uniform atomic_uint uElementsWritten;

uniform uint uMaxOutputs;
uniform uint uNumVertices;

const vec3 LIGHT_DIR = normalize(vec3(1, 2, -3));
const float SHADOW_LENGTH = 10000;

void emit(vec3 a, vec3 b, vec3 c, vec3 n) {

}

void main() {
    uint index = gl_GlobalInvocationID.x;
    if (index >= uNumVertices)
        return;
    atomicCounterIncrement(uElementsWritten);
}
