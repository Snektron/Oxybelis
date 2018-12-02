#include "planet/render/Skybox.h"
#include <vector>
#include <picopng/picopng.h>

Skybox::Skybox(GLuint binding, const std::array<Resource, 6> faces) {
    glActiveTexture(binding);
    this->texture.bind(GL_TEXTURE_CUBE_MAP);

    std::vector<unsigned char> pixel_data;

    for (int i = 0; i < 6; ++i) {
        const Resource& rs = faces[i];
        unsigned long w, h;
        decodePNG(pixel_data, w, h, reinterpret_cast<const unsigned char*>(rs.data()), rs.size());
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data.data());
        pixel_data.clear();
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
}
