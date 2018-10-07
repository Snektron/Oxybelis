#include <iostream>
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
#include "graphics/camera/FreeCam.h"
#include "graphics/planet/Planet.h"
#include "graphics/planet/PlanetRenderer.h"
#include "input/InputContext.h"
#include "input/InputManager.h"
#include "input/device/Mouse.h"
#include "input/device/Keyboard.h"
#include "assets.h"

enum class Input {
    Button,
    Vertical,
    Horizontal,
    Strafe,
    Forward,
    Fly,
    Rotate
};

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

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    auto manager = InputManager<Input>();

    auto kb = Keyboard<Input>(manager, window);
    kb.bind_action(Input::Button, GLFW_KEY_ESCAPE);
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

    ctx.connect_action(Input::Button, [&](Action) {
        esc = true;
    });

    manager.switch_context(ctx);

    auto projection = Perspective(1.0, 1.17f, 0.01f, 20.f);

    auto cam = FreeCam(QuatF::identity(), Vec3F(0, 0, -10));

    ctx.connect_axis(Input::Vertical, [&](double v){
        cam.rotate_pitch(v);
    });

    ctx.connect_axis(Input::Horizontal, [&](double v){
        cam.rotate_yaw(v);
    });

    ctx.connect_axis(Input::Rotate, [&](double v){
        cam.rotate_roll(v);
    });

    ctx.connect_axis(Input::Strafe, [&](double v){
        cam.strafe(0.05 * v);
    });

    ctx.connect_axis(Input::Fly, [&](double v){
        cam.fly(0.05 * v);
    });

    ctx.connect_axis(Input::Forward, [&](double v){
        cam.forward(0.05 * v);
    });

    auto p = Planet{
        TransformF(
            Vec3F(0.f, 0.f, 0.f),
            Vec3F(1.f),
            QuatF::identity()
        ),
        5
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
