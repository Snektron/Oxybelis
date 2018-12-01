#include "planet/chunk/Chunk.h"

Chunk::Chunk(const TerrainData& terrain):
    loc(terrain.loc), lod(terrain.lod), vertices(terrain.terrain_data.size()), center(loc.corners.center()) {
    this->vao.bind();
    this->terrain.bind(GL_ARRAY_BUFFER);
    Buffer::upload_data(GL_ARRAY_BUFFER, GL_STATIC_DRAW, terrain.terrain_data);

    auto sz = sizeof(TerrainData::VertexData);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sz, reinterpret_cast<void*>(offsetof(TerrainData::VertexData, vertex)));
    this->vao.enable_attrib(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sz, reinterpret_cast<void*>(offsetof(TerrainData::VertexData, normal)));
    this->vao.enable_attrib(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sz, reinterpret_cast<void*>(offsetof(TerrainData::VertexData, color)));
    this->vao.enable_attrib(2);

    this->vram_usage = terrain.terrain_data.size() * sizeof(TerrainData::VertexData);
}
