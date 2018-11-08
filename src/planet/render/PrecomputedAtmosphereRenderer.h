#ifndef _OXYBELIS_PLANET_RENDER_PRECOMPUTEDATMOSPHERERENDERER_H
#define _OXYBELIS_PLANET_RENDER_PRECOMPUTEDATMOSPHERERENDERER_H

#include <memory>
#include "atmosphere/model.h"
#include "graphics/shader/Program.h"
#include "graphics/camera/Camera.h"
#include "graphics/models/Quad.h"
#include "math/Mat.h"

class PrecomputedAtmosphereRenderer {
    std::unique_ptr<atmosphere::Model> model;

    Program shader;
    Uniform inv_proj_mat;
    Uniform model_mat;
    Uniform camera_origin;
    Uniform camera_up;
    Uniform camera_dir;

    Quad quad;

public:
    PrecomputedAtmosphereRenderer(double radius, double outer_radius);
    void render(const Mat4F& inv_proj, const Camera& cam) const;
};

#endif
