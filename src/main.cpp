#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdint>
#include <chrono>
#include <thread>
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "core/Window.h"
#include "math/Vec.h"
#include "math/Quat.h"
#include "math/Transform.h"
#include "graphics/Error.h"
#include "graphics/FrameBuffer.h"
#include "graphics/camera/Camera.h"
#include "graphics/camera/Projection.h"
#include "graphics/FrameBuffer.h"
#include "graphics/RenderBuffer.h"
#include "graphics/Texture.h"
#include "input/InputContext.h"
#include "input/InputManager.h"
#include "input/device/Mouse.h"
#include "input/device/Keyboard.h"
#include "planet/terragen/TerrainGenerator.h"
#include "planet/render/TerrainRenderer.h"
#include "planet/render/AtmosphereRenderer.h"
#include "planet/render/PrecomputedAtmosphereRenderer.h"
#include "planet/Planet.h"
#include "utility/ThreadPool.h"

enum class Input {
    Quit,
    Vertical,
    Horizontal,
    Strafe,
    Forward,
    Fly,
    Rotate,
    TogglePolygonMode,
    ToggleCullFace,
    SpeedUp,
    SpeedDown
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
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = Window(1280, 800, "Oxybelis");
    window.make_context_current();

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback([](GLenum, GLenum, GLuint, GLenum, GLsizei length, const GLchar* message, const void*){
        std::cout << "[OpenGL] ";
        std::cout.write(message, length) << std::endl;
    }, nullptr);

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
    kb.bind_action(Input::SpeedUp, GLFW_KEY_UP);
    kb.bind_action(Input::SpeedDown, GLFW_KEY_DOWN);

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

    auto projection = Perspective(1.0, 1.17f, 0.01f, 10'000.0_km);

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

    constexpr double cam_base_speed = 10.0_km;
    double cam_speed = 0;
    int speed = 0;

    auto update_speed = [&]() {
        cam_speed = cam_base_speed * std::pow(10, speed);
        std::cout << "Cam speed: " << (cam_speed * 60) << " m/s" << std::endl;
    };

    update_speed();

    ctx.connect_axis(Input::Strafe, [&](double v){
        cam.strafe(cam_speed * v);
    });

    ctx.connect_axis(Input::Fly, [&](double v){
        cam.fly(cam_speed * v);
    });

    ctx.connect_axis(Input::Forward, [&](double v){
        cam.forward(cam_speed * v);
    });

    ctx.connect_action(Input::SpeedUp, [&](Action a) {
        if (a == Action::Press) {
            ++speed;
            update_speed();
        }
    });

    ctx.connect_action(Input::SpeedDown, [&](Action a) {
        if (a == Action::Press) {
            --speed;
            update_speed();
        }
    });

    GLenum polymode = GL_FILL;
    ctx.connect_action(Input::TogglePolygonMode, [&](Action a) {
        if (a == Action::Press) {
            if (polymode == GL_LINE)
                polymode = GL_FILL;
            else
                polymode = GL_LINE;
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

    ThreadPool pool(std::thread::hardware_concurrency() * 3 / 4);
    auto gen = TerrainGenerator(pool);
    auto tr = TerrainRenderer(gen, p);

    auto atmos = PrecomputedAtmosphereRenderer(p.radius, p.radius * 1.0094);
    // auto atmos = AtmosphereRenderer();

    auto screen = FrameBuffer::screen();

    struct FrameBufferState {
        FrameBuffer fb;
        Texture color, distance;
        RenderBuffer depth;
    } fb_state;

    auto current_dim = Vec2I(window.dimensions());
    auto resize = [&] {
        glViewport(0, 0, current_dim.x, current_dim.y);

        projection.resize(current_dim);
        fb_state.fb = FrameBuffer();
        fb_state.fb.bind();

        glActiveTexture(GL_TEXTURE0);
        fb_state.color = Texture();
        fb_state.color.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, current_dim.x, current_dim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fb_state.color, 0);  

        glActiveTexture(GL_TEXTURE1);
        fb_state.distance = Texture();
        fb_state.distance.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, current_dim.x, current_dim.y, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, fb_state.distance, 0);

        fb_state.depth = RenderBuffer(GL_DEPTH_COMPONENT32, current_dim.x, current_dim.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb_state.depth);

        GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, buffers);

        screen.bind();
    };

    resize();

    while (!window.should_close() && !esc) {
        auto dim = Vec2I(window.dimensions());
        if (dim.x != current_dim .x || dim.y != current_dim.y) {
            current_dim = dim;
            resize();
        }

        fb_state.fb.bind();     
        glEnable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, polymode);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        tr.update_viewpoint(cam);
        tr.render(projection.to_matrix(), cam);

        screen.bind();
        glDisable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        atmos.render(projection.to_inverse_matrix(), cam);

        window.swap_buffers();
        assert_gl();
        glfwPollEvents();
        manager.update();
    }

    return 0;
}
