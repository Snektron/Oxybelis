#include "Oxybelis.h"
#include <iostream>
#include <thread>
#include <limits>
#include <random>
#include "input/utility.h"
#include "utility/units.h"
#include "utility/utility.h"

namespace {
    constexpr const float FIELD_OF_VIEW = 67.0_deg;
    constexpr const float NEAR = 0.01f;
    constexpr const float FAR = 10'000.0_km;

    constexpr const Vec3D PLANET_LOCATION = Vec3D(0);
    constexpr const double PLANET_RADIUS = 6'371.0_km;

    constexpr const double ATMOSPHERE_RADIUS = PLANET_RADIUS * 1.0094;

    constexpr const Vec3D CAMERA_START = Vec3D(0, 0, -10'000.0_km);
    constexpr const double CAMERA_BASE_SPEED = 100.0_km;

    constexpr const double CAMERA_ROLL_SPEED = 200;
}

Oxybelis::FrameBufferState::FrameBufferState(const Vec2I& dim):
    depth(GL_DEPTH_COMPONENT32, dim.x, dim.y),
    shadow_depth(GL_DEPTH_COMPONENT32, dim.x, dim.y) {

    // Terrain FBO
    {
        this->terrain_fb.bind();
        glViewport(0, 0, dim.x, dim.y);

        glActiveTexture(GL_TEXTURE0);
        this->color.bind();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dim.x, dim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->color, 0);

        glActiveTexture(GL_TEXTURE1);
        this->distance.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, dim.x, dim.y, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, this->distance, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depth);

        GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, buffers);
    }

    // Shadow FBO
    {
        this->shadow_fb.bind();
        glViewport(0, 0, dim.x, dim.y);

        glActiveTexture(GL_TEXTURE6); // 3 4 5 taken by atmosphere
        this->dndz.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, dim.x, dim.y, 0, GL_RG, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->dndz, 0);

        glActiveTexture(GL_TEXTURE7);
        this->zminmax.bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, dim.x, dim.y, 0, GL_RG, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, this->zminmax, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->shadow_depth);

        GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, buffers);
    }

    FrameBuffer::screen().bind();
    glViewport(0, 0, dim.x, dim.y);
}

Oxybelis::Oxybelis(Mouse<Input>& mouse, const Vec2I& dim):
    thread_pool(std::thread::hardware_concurrency() / 2),
    mouse(mouse), cursor_captured(false), quit(false),
    projection(dim, FIELD_OF_VIEW, NEAR, FAR),
    camera(QuatD::identity(), CAMERA_START),
    camera_speed_modifier(2),
    planet{PLANET_LOCATION, PLANET_RADIUS},
    atmos(0, 1, 6, 7, PLANET_RADIUS, ATMOSPHERE_RADIUS),
    terragen(this->thread_pool, earthlike::PointGenerator(std::random_device{}()), earthlike::TriangleGenerator{}),
    terrain(this->planet, this->terragen),
    shadow(1), // GL_TEXTURE1
    fb_state(dim) {

    this->update_camera_speed();

    this->input_ctx.connect_action(
        Input::ToggleMouse,
        filter_fn(Action::Press, std::bind(&Oxybelis::toggle_cursor, this))
    );

    this->input_ctx.connect_action(
        Input::Quit,
        [this](Action) {
            this->quit = true;
        }
    );

    auto change_speed = [this](int v) {
        this->camera_speed_modifier += v;
        this->update_camera_speed();
    };

    this->input_ctx.connect_action(Input::SpeedUp, filter_fn(Action::Press, std::bind(change_speed, 1)));
    this->input_ctx.connect_action(Input::SpeedDown, filter_fn(Action::Press, std::bind(change_speed, -1)));

    this->connect_camera();
    // this->toggle_cursor();
}

bool Oxybelis::update([[maybe_unused]] double dt) {
    this->terrain.update(this->camera);
    return this->quit;
}

void Oxybelis::resize(const Vec2I& dim) {
    this->projection.resize(dim);
    this->fb_state = FrameBufferState(dim);
}

void Oxybelis::render() {
    auto proj = this->projection.to_matrix();

    // Render terrain to its framebuffer
    {
        this->fb_state.terrain_fb.bind();     
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        this->terraren.render(this->terrain, proj, this->camera);
    }

    // Render shadow to its framebuffer
    {
        auto patch = this->terrain.current_patch();
        if (patch) {
            this->fb_state.shadow_fb.bind();     
            GLfloat dndz_clear[] = {0, 0, 0, 0};
            GLfloat zminmax_clear[] = {std::numeric_limits<GLfloat>::lowest(), 0, 0, 0};

            glClearBufferfv(GL_COLOR, 0, dndz_clear);
            glClearBufferfv(GL_COLOR, 1, zminmax_clear);
            glClear(GL_DEPTH_BUFFER_BIT);

            glDepthMask(GL_FALSE);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);

            glBlendEquationi(0, GL_FUNC_ADD);
            glBlendEquationi(1, GL_MAX);

            this->shadow.render(patch.value(), proj, this->camera);

            glDepthMask(GL_TRUE);
            glEnable(GL_CULL_FACE);
            glDisable(GL_BLEND);
        }
    }

    // Finally render everything else
    {
        FrameBuffer::screen().bind();
        glDisable(GL_DEPTH_TEST);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        this->atmos.render(this->projection.to_inverse_matrix(), this->camera);
        glEnable(GL_DEPTH_TEST);
    }
}

InputContext<Input>& Oxybelis::input_context() {
    return this->input_ctx;
}

void Oxybelis::toggle_cursor() {
    this->mouse.disable_cursor(this->cursor_captured ^= true);
}

void Oxybelis::connect_camera() {
    auto look_fn = [this](auto method) {
        auto f = std::bind(method, &this->camera, std::placeholders::_1);
        return [this, f](double v) {
            if (this->cursor_captured)
                f(v);
        };
    };

    this->input_ctx.connect_axis(Input::Vertical, look_fn(&Camera::rotate_pitch));
    this->input_ctx.connect_axis(Input::Horizontal, look_fn(&Camera::rotate_yaw));

    this->input_ctx.connect_axis(Input::Rotate, [this](double v, double dt) {
        if (this->cursor_captured) {
            this->camera.rotate_roll(v * dt * CAMERA_ROLL_SPEED);
        }
    });

    auto move_fn = [this](auto method) {
        auto f = std::bind(method, &this->camera, std::placeholders::_1);
        return [this, f](double v, double dt) {
            if (this->cursor_captured)
                f(v * dt * this->camera_speed);
        };
    };

    this->input_ctx.connect_axis(Input::Strafe, move_fn(&Camera::strafe));
    this->input_ctx.connect_axis(Input::Fly, move_fn(&Camera::fly));
    this->input_ctx.connect_axis(Input::Forward, move_fn(&Camera::forward));
}

void Oxybelis::update_camera_speed() {
    this->camera_speed = CAMERA_BASE_SPEED * std::pow(10, this->camera_speed_modifier);
    std::cout << "Cam speed: " << this->camera_speed << " m/s" << std::endl;
}
