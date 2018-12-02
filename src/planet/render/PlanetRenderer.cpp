#include "planet/render/PlanetRenderer.h"
#include "planet/Planet.h"
#include "planet/render/AtmosphereRenderer.h"
#include "graphics/FrameBuffer.h"

PlanetRenderer::PlanetRenderer(const Vec2UI& dim):
    terra_ren(dim),
    shadow_ren(dim, 1) {
}

void PlanetRenderer::resize(const Vec2UI& dim) {
    this->terra_ren.resize(dim);
    this->shadow_ren.resize(dim);
}

void PlanetRenderer::render(const Planet& planet, const AtmosphereRenderer& atmos, const RenderInfo& info) {
    if (!planet.has_drawable_terrain())
        return;

    auto cam = Camera(info.cam);
    cam.translation -= planet.translation;

    auto translated_info = RenderInfo {
        cam,
        info.proj,
        info.inv_proj,
        cam.to_view_matrix()
    };

    // Render terrain to its framebuffer
    this->terra_ren.render(planet, translated_info);

    // Render shadow to its framebuffer
    this->shadow_ren.render(planet, translated_info);

    // Finally render everything else
    {
        FrameBuffer::screen().bind();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        atmos.render(translated_info);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }
}
