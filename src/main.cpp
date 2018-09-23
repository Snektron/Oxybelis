#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <cmath>
#include <cstdint>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
        Horizontal
    };

    InputManager<Input> manager;

    Keyboard<Input> kb(manager, window);
    kb.bind_axis(Input::Vertical, GLFW_KEY_W, -1.0);
    kb.bind_axis(Input::Vertical, GLFW_KEY_S, 1.0);
    kb.bind_axis(Input::Horizontal, GLFW_KEY_A, -1.0);
    kb.bind_axis(Input::Horizontal, GLFW_KEY_D, 1.0);
    kb.bind_action(Input::Button, GLFW_KEY_ESCAPE);

    Mouse<Input> mouse(manager, window);

    InputContext<Input> ctx;

    bool esc = false;

    ctx.connect_action(Input::Button, [&](Action) {
        esc = true;
    });

    manager.switch_context(ctx);

    auto qa = QuatF::identity();

    TransformF t(
        Vec3F(0.f, 0.f, 0.f),
        Vec3F(2.f, 1.f, 1.f),
        qa
    );

    auto cam = mat::look_at(
        Vec3F(5, 5, 5),
        Vec3F(0, 0, 0),
        Vec3F(0, 1, 0)
    );

    while (!window.should_close() && !esc)
    {
        auto dim = window.dimensions();
        glViewport(0, 0, dim.x, dim.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto perspective = mat::perspective(static_cast<float>(dim.x) / dim.y, 1.17f, 0.1f, 50.f);

        //t.rotation = smix(qa, qb, float(std::sin(glfwGetTime()) * 0.5 + 0.5));

        //t.rotation *= QuatF::identity();
        //t.rotation.normalize();

        float x = float(std::sin(glfwGetTime()) * 0.5 + 0.5) * float(M_PI) / 2.f;
        float y = float(glfwGetTime());
        t.rotation *= QuatF(0, 0.01f, 0, 1).normalize() * QuatF(0, 0, 0.01f, 1).normalize();

        glUniformMatrix4fv(uModel, 1, GL_FALSE, (cam * t.to_matrix()).data());
    
        glUniformMatrix4fv(uPerspective, 1, GL_FALSE, perspective.data());
        glDrawElementsInstanced(GL_TRIANGLES, sizeof(INDICES) / sizeof(uint8_t), GL_UNSIGNED_BYTE, 0, instances);

        window.swap_buffers();
        assert_gl();
        glfwPollEvents();
        manager.update();
    }

    return 0;
}
