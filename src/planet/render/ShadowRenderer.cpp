#include "planet/render/ShadowRenderer.h"
#include "graphics/shader/ProgramBuilder.h"
#include "assets.h"

namespace {
    Program load_shader() {
        return ProgramBuilder()
            .with(ShaderType::Compute, assets::shadow_glsl)
            .link();
    }
}

ShadowRenderer::ShadowRenderer():
    shadow_compute(load_shader()) {

}
