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

    constexpr const Vec3D PLANET_LOCATION = Vec3D(-10'000.0_km, 0, 0);
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
    terragen(this->thread_pool, earthlike::PointGenerator(std::random_device{}()), earthlike::TriangleGenerator{}),
    planet(PLANET_LOCATION, PLANET_RADIUS, this->terragen),
    atmos(0, 1, 6, 7, PLANET_RADIUS, ATMOSPHERE_RADIUS),
    planet_renderer(dim) {

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
    this->planet.update(this->camera);
    return this->quit;
}

void Oxybelis::resize(const Vec2UI& dim) {
    this->projection.resize(dim);
    this->planet_renderer.resize(dim);

    FrameBuffer::screen().bind();
    glViewport(0, 0, dim.x, dim.y);
}

void Oxybelis::render() {
    if (!this->planet.has_drawable_terrain())
        return;

    auto proj = this->projection.to_matrix();
    auto inv_proj = this->projection.to_inverse_matrix();

    this->planet_renderer.render(this->planet, this->atmos, proj, inv_proj, this->camera);
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
