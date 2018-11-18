#include "Oxybelis.h"
#include <iostream>
#include <thread>
#include "input/utility.h"
#include "utility/units.h"
#include "utility/utility.h"

namespace {
    constexpr const float FIELD_OF_VIEW = 67.0_deg;
    constexpr const float NEAR = 0.01f;
    constexpr const float FAR = 10'000.0_km;

    constexpr const Vec3D CAMERA_START = Vec3D(0, 0, -10'000.0_km);
    constexpr const double CAMERA_BASE_SPEED = 10.0_km;

    constexpr const Vec3D PLANET_LOCATION = Vec3D(0);
    constexpr const double PLANET_RADIUS = 6'371.0_km;

    constexpr const double ATMOSPHERE_RADIUS = PLANET_RADIUS * 1.0094;
}

Oxybelis::FrameBufferState::FrameBufferState(const Vec2I& dim):
    depth(GL_DEPTH_COMPONENT32, dim.x, dim.y) {
    this->fb.bind();
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
    atmos(PLANET_RADIUS, ATMOSPHERE_RADIUS),
    terragen(this->thread_pool),
    terraren(this->terragen, this->planet),
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
    this->toggle_cursor();
}

bool Oxybelis::update([[maybe_unused]] double dt) {
    return this->quit;
}

void Oxybelis::resize(const Vec2I& dim) {
    this->projection.resize(dim);
    this->fb_state = FrameBufferState(dim);
}

void Oxybelis::render() {
    this->fb_state.fb.bind();     
    glEnable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto proj = this->projection.to_matrix();

    this->terraren.update_viewpoint(this->camera);
    this->terraren.render(proj, this->camera);

    FrameBuffer::screen().bind();
    glDisable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    this->atmos.render(this->projection.to_inverse_matrix(), this->camera);


    auto opt_patch = this->terraren.current_patch();
    if (opt_patch) {
        this->shadow.begin();
        auto patch = opt_patch.value();

        for (auto& entry : patch.get().chunks) {
            if (entry->is_ready()) {
                this->shadow.dispatch(this->camera, entry->chunk());
            }
        }

        this->shadow.end(proj, this->camera);
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
    this->input_ctx.connect_axis(Input::Rotate, look_fn(&Camera::rotate_roll));

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
