#ifndef _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H
#define _OXYBELIS_PLANET_RENDER_PLANETRENDERER_H

#include "planet/render/TerrainRenderer.h"
#include "planet/render/ShadowRenderer.h"
#include "math/Vec.h"

class Planet;
class AtmosphereRenderer;

class PlanetRenderer {
    TerrainRenderer terra_ren;
    ShadowRenderer shadow_ren;

public:
    PlanetRenderer(const Vec2UI& dim);
    void resize(const Vec2UI& dim);
    void render(const Planet& planet, const AtmosphereRenderer& atmos, const Mat4F& proj, const Mat4F& inv_proj, Camera cam);
};

#endif
