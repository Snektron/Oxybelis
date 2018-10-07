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
#include "input/InputContext.h"
#include "input/InputManager.h"
#include "input/device/Mouse.h"
#include "input/device/Keyboard.h"
#include "assets.h"

const float VERTICES[] = {
    0, -0.525731, 0.850651,
    0.850651, 0, 0.525731,
    0.850651, 0, -0.525731,
    -0.850651, 0, -0.525731,
    -0.850651, 0, 0.525731,
    -0.525731, 0.850651, 0,
    0.525731, 0.850651, 0,
    0.525731, -0.850651, 0,
    -0.525731, -0.850651, 0,
    0, -0.525731, -0.850651,
    0, 0.525731, -0.850651,
    0, 0.525731, 0.850651
};

const uint8_t INDICES[] = {
    1, 2, 6,
    1, 7, 2,
    3, 4, 5,
    4, 3, 8,
    6, 5, 11,
    5, 6, 10,
    9, 10, 2,
    10, 9, 3,
    7, 8, 9,
    8, 7, 0,
    11, 0, 1,
    0, 11, 4,
    6, 2, 10,
    1, 6, 11,
    3, 5, 10,
    5, 4, 11,
    2, 7, 9,
    7, 1, 0,
    3, 9, 8,
    4, 8, 0,
};

int main() {
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "Failed to initializ GLFW" << std::endl;
        return 1;
    }

    auto _ = defer([](){
        glfwTerminate();
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window window(1280, 800, "Oxybelis");
    window.make_context_current();

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    
    glClearColor(.97f, .97f, .97f, .97f);

    auto program = ProgramBuilder()
        .with(ShaderType::Vertex, assets::passthrough_vs)
        .with(ShaderType::Fragment, assets::passthrough_fs)
        .link();

    Attribute aVertex = program.attribute("aVertex");

    Uniform uPerspective = program.uniform("uPerspective");
    Uniform uModel = program.uniform("uModel");
    Uniform uNumInstances = program.uniform("uNumInstances");
    program.use();

    VertexArray vao;
    vao.bind();

    Buffer vertices = Buffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, VERTICES);
    glVertexAttribPointer(aVertex, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enable_attrib(aVertex);
    
    auto indices = Buffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, INDICES);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    const GLuint instances = 1;
    glUniform1f(uNumInstances, float(instances));

    enum class Input {
        Button,
        Vertical,
        Horizontal,
        Strafe,
        Forward,
        Fly,
        Rotate
    };

    auto manager = InputManager<Input>();

    Keyboard<Input> kb(manager, window);
    kb.bind_action(Input::Button, GLFW_KEY_ESCAPE);
    kb.bind_axis(Input::Strafe, GLFW_KEY_D, 1.0);
    kb.bind_axis(Input::Strafe, GLFW_KEY_A, -1.0);
    kb.bind_axis(Input::Forward, GLFW_KEY_W, 1.0);
    kb.bind_axis(Input::Forward, GLFW_KEY_S, -1.0);
    kb.bind_axis(Input::Fly, GLFW_KEY_SPACE, 1.0);
    kb.bind_axis(Input::Fly, GLFW_KEY_LEFT_SHIFT, -1.0);
    kb.bind_axis(Input::Rotate, GLFW_KEY_Q, -0.01);
    kb.bind_axis(Input::Rotate, GLFW_KEY_E, 0.01);

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

    auto trans = TransformF(
        Vec3F(0.f, 0.f, 0.f),
        Vec3F(1.f),
        QuatF::identity()
    );

    auto projection = Perspective(1.0, 1.17f, 0.1f, 50.f);

    auto cam = FreeCam(QuatF::identity(), Vec3F(0, 0, 10));

    ctx.connect_axis(Input::Vertical, [&](double v){
        cam.rotate_pitch(-v);
    });

    ctx.connect_axis(Input::Horizontal, [&](double v){
        cam.rotate_yaw(-v);
    });

    ctx.connect_axis(Input::Rotate, [&](double v){
        cam.rotate_roll(-v);
    });

    ctx.connect_axis(Input::Strafe, [&](double v){
        cam.strafe(0.05 * v);
    });

    ctx.connect_axis(Input::Fly, [&](double v){
        cam.fly(0.05 * v);
    });

    ctx.connect_axis(Input::Forward, [&](double v){
        cam.forward(-0.05 * v);
    });

    while (!window.should_close() && !esc)
    {
        auto dim = window.dimensions();
        glViewport(0, 0, dim.x, dim.y);
        projection.resize(dim);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        trans.rotation *= QuatF::axis_angle(0, 1, 0, 0.01f);

        glUniformMatrix4fv(uModel, 1, GL_FALSE, (cam.to_view_matrix() * trans.to_matrix()).data());
    
        glUniformMatrix4fv(uPerspective, 1, GL_FALSE, projection.to_matrix().data());
        glDrawElements(GL_TRIANGLES, sizeof(INDICES) / sizeof(uint8_t), GL_UNSIGNED_BYTE, 0);

        window.swap_buffers();
        assert_gl();
        glfwPollEvents();
        manager.update();
    }

    return 0;
}
