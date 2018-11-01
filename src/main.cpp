#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdint>
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "core/Window.h"
#include "math/Vec.h"
#include "math/Mat.h"
#include "math/Quat.h"
#include "math/Transform.h"
#include "graphics/Error.h"
#include "graphics/GlObject.h"
#include "graphics/VertexArray.h"
#include "graphics/Buffer.h"
#include "graphics/shader/ProgramBuilder.h"
#include "graphics/camera/Projection.h"
#include "graphics/camera/Camera.h"
#include "input/InputContext.h"
#include "input/InputManager.h"
#include "input/device/Mouse.h"
#include "input/device/Keyboard.h"
#include "planet/render/PlanetRenderer.h"
#include "planet/Planet.h"
#include "assets.h"

enum class Input {
    Quit,
    Vertical,
    Horizontal,
    Strafe,
    Forward,
    Fly,
    Rotate,
    TogglePolygonMode,
    ToggleCullFace
};

constexpr long double operator ""_Mm(long double val) {
    return val * 1'000'000.0;
}

constexpr long double operator ""_km(long double val) {
    return val * 1'000.0;
}

constexpr long double operator ""_m(long double val) {
    return val;
}

int main() {
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    auto _ = defer([](){
        glfwTerminate();
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = Window(1280, 800, "Oxybelis");
    window.make_context_current();

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    
    glClearColor(.97f, .97f, .97f, .97f);

    glEnable(GL_DEPTH_TEST);
    // glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

    auto manager = InputManager<Input>();

    auto kb = Keyboard<Input>(manager, window);
    kb.bind_action(Input::Quit, GLFW_KEY_ESCAPE);
    kb.bind_action(Input::TogglePolygonMode, GLFW_KEY_F);
    kb.bind_action(Input::ToggleCullFace, GLFW_KEY_R);
    kb.bind_axis(Input::Strafe, GLFW_KEY_D, 1.0);
    kb.bind_axis(Input::Strafe, GLFW_KEY_A, -1.0);
    kb.bind_axis(Input::Forward, GLFW_KEY_W, 1.0);
    kb.bind_axis(Input::Forward, GLFW_KEY_S, -1.0);
    kb.bind_axis(Input::Fly, GLFW_KEY_SPACE, 1.0);
    kb.bind_axis(Input::Fly, GLFW_KEY_LEFT_SHIFT, -1.0);
    kb.bind_axis(Input::Rotate, GLFW_KEY_Q, 0.01);
    kb.bind_axis(Input::Rotate, GLFW_KEY_E, -0.01);

    auto mouse = Mouse<Input>(manager, window);
    mouse.bind_axis(Input::Horizontal, MouseAxis::Horizontal, 0.01);
    mouse.bind_axis(Input::Vertical, MouseAxis::Vertical, 0.01);
    mouse.disable_cursor();

    auto ctx = InputContext<Input>();

    bool esc = false;

    ctx.connect_action(Input::Quit, [&](Action) {
        esc = true;
    });

    manager.switch_context(ctx);

    auto projection = Perspective(1.0, 1.17f, 0.01f, 100.0_Mm);

    auto cam = Camera(QuatD::identity(), Vec3D(0, 0, -10'000.0_km));

    ctx.connect_axis(Input::Vertical, [&](double v){
        cam.rotate_pitch(v);
    });

    ctx.connect_axis(Input::Horizontal, [&](double v){
        cam.rotate_yaw(v);
    });

    ctx.connect_axis(Input::Rotate, [&](double v){
        cam.rotate_roll(v);
    });

    constexpr double cam_speed = 10.0_km;

    ctx.connect_axis(Input::Strafe, [&](double v){
        cam.strafe(cam_speed * v);
    });

    ctx.connect_axis(Input::Fly, [&](double v){
        cam.fly(cam_speed * v);
    });

    ctx.connect_axis(Input::Forward, [&](double v){
        cam.forward(cam_speed * v);
    });

    GLenum polymode = GL_FILL;
    glPolygonMode(GL_FRONT_AND_BACK, polymode);
    ctx.connect_action(Input::TogglePolygonMode, [&](Action a) {
        if (a == Action::Press) {
            if (polymode == GL_LINE)
                polymode = GL_FILL;
            else
                polymode = GL_LINE;
            glPolygonMode(GL_FRONT_AND_BACK, polymode);
        }
    });

    glEnable(GL_CULL_FACE);
    ctx.connect_action(Input::ToggleCullFace, [&](Action a) {
        if (a == Action::Press) {
            if (glIsEnabled(GL_CULL_FACE))
                glDisable(GL_CULL_FACE);
            else
                glEnable(GL_CULL_FACE);
        }
    });

    auto p = Planet {
        Vec3D(0, 0, 0),
        6'371.0_km
    };

    auto pr = PlanetRenderer(p);

    while (!window.should_close() && !esc) {
        auto dim = window.dimensions();
        glViewport(0, 0, dim.x, dim.y);
        projection.resize(dim);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pr.render(projection.to_matrix(), cam);

        window.swap_buffers();
        assert_gl();
        glfwPollEvents();
        manager.update();
    }

    return 0;
}
