#define GLM_ENABLE_EXPERIMENTAL
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
#include "math/Fixed.h"
#include "graphics/Error.h"
#include "graphics/GlObject.h"
#include "graphics/VertexArray.h"
#include "graphics/Buffer.h"
#include "graphics/shader/ProgramBuilder.h"
#include "graphics/Projection.h"
#include "graphics/FreeCam.h"
#include "input/InputContext.h"
#include "input/InputManager.h"
#include "input/device/Mouse.h"
#include "input/device/Keyboard.h"
#include "assets.h"

float VERTICES[] = {
    -1, -1, -1,
     1, -1, -1,
    -1,  1, -1,
     1,  1, -1,

    -1, -1,  1,
     1, -1,  1,
    -1,  1,  1,
     1,  1,  1,
};

uint8_t INDICES[] = {
    // back
    0, 3, 1,
    0, 2, 3,

    // front
    4, 5, 7,
    4, 7, 6,

    // bottom
    0, 1, 4,
    4, 1, 5,

    // top
    2, 6, 3,
    3, 6, 7,

    // left
    0, 4, 2,
    4, 6, 2,

    // right
    1, 3, 5,
    5, 3, 7
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

    Window window(640, 480, "Oxybelis");
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

    auto vertices = Buffer::make_static(VERTICES, sizeof(VERTICES) / sizeof(float));
    glVertexAttribPointer(aVertex, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enable_attrib(aVertex);

    auto indices = Buffer::make_static(INDICES, sizeof(INDICES) / sizeof(uint8_t), GL_ELEMENT_ARRAY_BUFFER);

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

    auto qa = QuatF::identity();

    auto trans = TransformF(
        Vec3F(0.f, 0.f, 0.f),
        Vec3F(2.f, 1.f, 1.f),
        qa
    );

    auto cam = FreeCam(QuatF::identity(), Vec3F(0, 0, 10));

    auto projection = Perspective(1.0, 1.17f, 0.1f, 50.f);

    ctx.connect_axis(Input::Vertical, [&](double v){
        cam.rotation *= quat::axis_angle<float>(1, 0, 0, -float(v));
        cam.rotation.normalize();
    });

    ctx.connect_axis(Input::Horizontal, [&](double v){
        cam.rotation *= quat::axis_angle<float>(0, 1, 0, -float(v));
        cam.rotation.normalize();
    });

    ctx.connect_axis(Input::Rotate, [&](double v){
        cam.rotation *= quat::axis_angle<float>(0, 0, 1, -float(v));
        cam.rotation.normalize();
    });

    ctx.connect_axis(Input::Strafe, [&](double v){
        cam.translation += cam.rotation.to_matrix().column(0).xyz * 0.05 * v;
    });

    ctx.connect_axis(Input::Fly, [&](double v){
        cam.translation += cam.rotation.to_matrix().column(1).xyz * 0.05 * v;
    });

    ctx.connect_axis(Input::Forward, [&](double v){
        cam.translation += cam.rotation.to_matrix().column(2).xyz * 0.05 * -v;
    });

    while (!window.should_close() && !esc)
    {
        auto dim = window.dimensions();
        glViewport(0, 0, dim.x, dim.y);
        projection.resize(dim);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        trans.rotation *= QuatF(0, 0.01f, 0, 1).normalize() * QuatF(0, 0, 0.01f, 1).normalize();

        glUniformMatrix4fv(uModel, 1, GL_FALSE, (cam.to_view_matrix() * trans.to_matrix()).data());
    
        glUniformMatrix4fv(uPerspective, 1, GL_FALSE, projection.to_matrix().data());
        glDrawElementsInstanced(GL_TRIANGLES, sizeof(INDICES) / sizeof(uint8_t), GL_UNSIGNED_BYTE, 0, instances);

        window.swap_buffers();
        assert_gl();
        glfwPollEvents();
        manager.update();
    }

    return 0;
}
