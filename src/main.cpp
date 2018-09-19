#include <iostream>
#include <cmath>
#include <cstdint>
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "core/Window.h"
#include "math/Matrix.h"
#include "graphics/Error.h"
#include "graphics/GlObject.h"
#include "graphics/VertexArray.h"
#include "graphics/Buffer.h"
#include "graphics/shader/ProgramBuilder.h"
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

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window window(640, 480, "Test Window");
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

    auto model = make_translation(0.f, 0.f, -5.f);
    auto rot = make_rotation(0.57f, 0.57f, 0.57f, 1.f / 40.f) * make_rotation(0.f, 1.f, 0.f, 1.f / 30.f);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    const GLuint instances = 100;
    glUniform1f(uNumInstances, float(instances));

    enum class Input {
        Test,
        Oof
    };

    InputManager<Input> manager;

    Keyboard<Input> kb(manager);
    kb.bind_action(Input::Test, GLFW_KEY_A);
    kb.bind_action(Input::Test, GLFW_KEY_B);
    kb.bind_axis(Input::Oof, GLFW_KEY_C, 1.0);
    kb.bind_axis(Input::Oof, GLFW_KEY_D, 1.0);

    Mouse<Input> mouse(manager);
    mouse.bind_action(Input::Test, MouseButton::Left);
    mouse.bind_axis(Input::Oof, MouseAxis::Vertical, 1.0);

    InputContext<Input> ctx;
    ctx.connect_action(Input::Test, [](Action a) {
        if (a == Action::Press)
            std::cout << "Action Press" << std::endl;
        else
            std::cout << "Action Release" << std::endl;
    });

    ctx.connect_axis(Input::Oof, [](double x) {
        std::cout << "Axis " << x << std::endl;
    });

    manager.switch_context(ctx);
    
    mouse.dispatch_button(MouseButton::Left, Action::Press);
    kb.dispatch(GLFW_KEY_B, Action::Release);

    kb.dispatch(GLFW_KEY_C, Action::Press);
    manager.update();
    kb.dispatch(GLFW_KEY_C, Action::Release);
    kb.dispatch(GLFW_KEY_D, Action::Press);
    manager.update();
    mouse.update_cursor(0.1, 0.1);
    manager.update();
    mouse.update_cursor(0.1, 0.1);
    manager.update();

    while (!window.should_close())
    {
        auto dim = window.dimensions();
        glViewport(0, 0, dim.x(), dim.y());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto perspective = make_perspective(static_cast<float>(dim.x()) / dim.y(), 1.17f, 0.1f, 50.f);

        model *= rot;

        glUniformMatrix4fv(uPerspective, 1, GL_FALSE, perspective.data());
        glUniformMatrix4fv(uModel, 1, GL_FALSE, model.data());

        glDrawElementsInstanced(GL_TRIANGLES, sizeof(INDICES) / sizeof(uint8_t), GL_UNSIGNED_BYTE, 0, instances);

        window.swap_buffers();
        glfwPollEvents();
        assert_gl();
    }

    return 0;
}
