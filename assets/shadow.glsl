#version 430

struct Vertex {
    vec3 position;
    vec3 normal;
    vec3 color;
};

layout(packed, binding = 0) buffer terrain {
    vec3 vertices[];
};

layout(packed, binding = 1) buffer shadow_volumes {
    uint max_elements;
    vec3 shadow_volumes[];
};

layout(binding = 2) uniform atomic_uint elements_written;

layout(local_size_x = 512, local_size_y = 1, local_size_z = 1) in;

void main() {

}
