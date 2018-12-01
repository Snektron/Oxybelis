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

Oxybelis::Oxybelis(Mouse<Input>& mouse, const Vec2UI& dim):
    thread_pool(std::thread::hardware_concurrency() / 2),
    mouse(mouse), cursor_captured(false), quit(false),
    projection(dim, FIELD_OF_VIEW, NEAR, FAR),
    camera(QuatD::identity(), CAMERA_START),
    camera_speed_modifier(2),
    planet{PLANET_LOCATION, PLANET_RADIUS},
    atmos(0, 1, 6, 7, PLANET_RADIUS, ATMOSPHERE_RADIUS),
    terragen(this->thread_pool, earthlike::PointGenerator(std::random_device{}()), earthlike::TriangleGenerator{}),
    terrain(this->planet, this->terragen),
    terraren(dim),
    shadow(dim, 1) {

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
}

bool Oxybelis::update([[maybe_unused]] double dt) {
    this->terrain.update(this->camera);
    return this->quit;
}

void Oxybelis::resize(const Vec2UI& dim) {
    this->projection.resize(dim);
    this->terraren.resize(dim);
    this->shadow.resize(dim);

    FrameBuffer::screen().bind();
    glViewport(0, 0, dim.x, dim.y);
}

void Oxybelis::render() {
    auto opt_patch = this->terrain.current_patch();
    if (!opt_patch)
        return;
    auto patch = opt_patch.value();

    auto proj = this->projection.to_matrix();
    auto inv_proj = this->projection.to_inverse_matrix();

    // Render terrain to its framebuffer
    this->terraren.render(patch, proj, this->camera);

    // Render shadow to its framebuffer
    this->shadow.render(patch, proj, this->camera);

    // Finally render everything else
    {
        FrameBuffer::screen().bind();
        glDisable(GL_DEPTH_TEST);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        this->atmos.render(inv_proj, this->camera);
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
