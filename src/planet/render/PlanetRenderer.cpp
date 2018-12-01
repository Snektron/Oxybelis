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

void PlanetRenderer::render(const Planet& planet, const AtmosphereRenderer& atmos, const Mat4F& proj, const Mat4F& inv_proj, Camera cam) {
    if (!planet.has_drawable_terrain())
        return;

    cam.translation -= planet.translation;

    // Render terrain to its framebuffer
    this->terra_ren.render(planet, proj, cam);

    // Render shadow to its framebuffer
    this->shadow_ren.render(planet, proj, cam);

    // Finally render everything else
    {
        FrameBuffer::screen().bind();
        glDisable(GL_DEPTH_TEST);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        atmos.render(inv_proj, cam);
        glEnable(GL_DEPTH_TEST);
    }
}
