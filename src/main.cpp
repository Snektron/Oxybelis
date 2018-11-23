#include <iostream>
#include <iomanip>
#include <thread>
#include <functional>
#include <cmath>
#include <cstdint>
#include <chrono>
#include <experimental/string_view>
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "core/Window.h"
#include "math/Vec.h"
#include "math/Quat.h"
#include "math/Transform.h"
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
#include "Oxybelis.h"
#include "OxybelisInput.h"

void run(Window& window, InputManager<Input>& manager, Mouse<Input>& mouse) {
    using std::chrono::high_resolution_clock;

    auto current_dim = Vec2I(window.dimensions());
    Oxybelis oxybelis(mouse, current_dim);
    manager.switch_context(oxybelis.input_context());

    auto start = high_resolution_clock::now();
    auto last_frame = start;
    size_t frames = 0;

    while (!window.should_close()) {
        auto dim = Vec2I(window.dimensions());
        if (dim.x != current_dim .x || dim.y != current_dim.y) {
            current_dim = dim;
            oxybelis.resize(dim);
        }

        oxybelis.render();

        window.swap_buffers();
        glfwPollEvents();

        ++frames;
        auto now = high_resolution_clock::now();
        std::chrono::duration<double> diff = now - start;
        if (diff > std::chrono::seconds{1}) {
            std::cout << "FPS: " << (frames / diff.count()) << std::endl;

            frames = 0;
            start = now;
        }

        diff = now - last_frame;
        last_frame = now;
        double dt = diff.count();

        manager.update(dt);
        if (oxybelis.update(dt))
            break;
    }
}

int main(int argc, char* argv[]) {
    bool quiet = false;

    for (int i = 0; i < argc; ++i) {
        auto arg = std::experimental::string_view(argv[i]);
        if (arg == "-q" || arg == "--no-debug")
            quiet = true;
    }

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
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = Window(1280, 800, "Oxybelis");
    window.make_context_current();

    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    glfwSwapInterval(0);

    glEnable(GL_CULL_FACE);

    if (!quiet) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback([](GLenum, GLenum, GLuint, GLenum, GLsizei length, const GLchar* message, const void*){
            std::cout << "[OpenGL] ";
            std::cout.write(message, length) << std::endl;
        }, nullptr);
    }

    auto manager = InputManager<Input>();

    auto mouse = Mouse<Input>(manager, window);
    auto kb = Keyboard<Input>(manager, window);
    initialize_input(mouse, kb);

    run(window, manager, mouse);

    return 0;
}
