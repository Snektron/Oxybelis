
void setup_input(InputManager<Input>& input_manager) {
    this->camera_context.connect_action(Input::Escape, Action::Press, [&](Action a) {
        input_manager.switch_context(pause_context);
        input_manager.mouse_captured(false);
    });

    this->camera_context.connect_action(Input::Forward, this->camera, &Camera::set_z_movement);
    this->camera_context.connect_action(Input::Fly, this->camera, &Camera::set_y_movement);
    this->camera_context.connect_action(Input::Strafe, this->camera, &Camera::set_x_movement);
    this->camera_context.connect_axis(Input::LookHoriz, this->camera, &Camera::rotate_theta);
    this->camera_context.connect_axis(Input::LookVert, this->camera, &Camera::rotate_phi);

    this->pause_context.connect_action(Input::Focus, Action::Press, [&](Action a) {
        input_manager.switch_context(camera_context);
        input_manager.mouse_captured(true);
    });
}

void map_input(Window& win, InputManager<Input>& input_manager) {
    input_manager.register(win);
    input_manager.bind_axis(win, GLFW_W, Input::Forward, 1);
    input_manager.bind_axis(win, GLFW_S, Input::Forward, -1);
    input_manager.bind_axis(win, GLFW_D, Input::Strafe, 1);
    input_manager.bind_axis(win, GLFW_A, Input::Strafe, -1);
    input_manager.bind_axis(win, GLFW_SPACE, Input::Fly, 1);
    input_manager.bind_axis(win, GLFW_SHIFT, Input::Fly, -1);
    input_manager.bind_action(win, GLFW_MOUSE_BUTTON_LEFT, Input::Fire);

    input_manager.bind_axis(win, Mouse::Horizontal, Input::LookHoriz, 1.0);
    input_manager.bind_axis(win, Mouse::Vertical, Input::LookVert, 1.0);

    input_manager.bind_action(win, GLFW_MOUSE_BUTTON_LEFT, Input::Focus);
    input_manager.bind_action(win, GLFW_ECS, Input::Escape);
}

input_manager.update();

// -----

template <typename T>
class AxisInfo {
    T axis_name;
    int key_value;
};

template <typename T = size_t>
class InputManager {
    std::unordered_multimap<Key, T> actions;

    std::unordered_multimap<Axis, size_t> axis_mapping;
    std::unordered_multimap<Key, size_t> axis_key_mapping;
    std::vector<AxisInfo> axes;

public:
};