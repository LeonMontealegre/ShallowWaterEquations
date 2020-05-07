#ifndef __INPUT_H__
#define __INPUT_H__

#include <GLFW/glfw3.h>
#include <leon/vector.h>
#include <vector>
#include "key.h"

class Input {
public:
	static const int LEFT_MOUSE_BUTTON = GLFW_MOUSE_BUTTON_1;
	static const int RIGHT_MOUSE_BUTTON = GLFW_MOUSE_BUTTON_2;
	static const int MIDDLE_MOUSE_BUTTON = GLFW_MOUSE_BUTTON_3;
	static const int MOUSE_BUTTON_4 = GLFW_MOUSE_BUTTON_4;
	static const int MOUSE_BUTTON_5 = GLFW_MOUSE_BUTTON_5;
	static const int MOUSE_BUTTON_6 = GLFW_MOUSE_BUTTON_6;
	static const int MOUSE_BUTTON_7 = GLFW_MOUSE_BUTTON_7;
	static const int LAST_MOUSE_BUTTON = GLFW_MOUSE_BUTTON_8;

    static void init(GLFWwindow* w) {
        glfwSetKeyCallback(w, key_callback);
        glfwSetCursorPosCallback(w, mouse_move_callback);
        glfwSetMouseButtonCallback(w, mouse_button_callback);

        // Reset variables
        for (int i = 0; i < NUM_KEYS; i++)
            first_pressed_keys[i] = pressed_keys[i] = released_keys[i] = false;
        for (int i = 0; i < 8; i++)
            mouse_first_pressed[i] = mouse_pressed[i] = mouse_released[i] = false;

        scroll_amt = 0;
        last_key_pressed = Key::NONE;
    }

    static Key get_last_key() { return last_key_pressed; }

    static bool get_key_down(Key key) {
        if (first_pressed_keys[(int)key]) {
            if (!contains_key(keys_to_change, (int)key))
                keys_to_change.push_back({(int)key, true});
            return true;
        }
        return false;
    }

    static bool get_key(Key key) {
        return pressed_keys[(int)key];
    }

    static bool get_key_up(Key key) {
        if (released_keys[(int)key]) {
            if (!contains_key(keys_to_change, (int)key))
                keys_to_change.push_back({(int)key, false});
            return true;
        }
        return false;
    }

    static bool get_mouse_down(int button) {
        if (mouse_first_pressed[button]) {
            if (!contains_key(mouse_vals_to_change, button))
                mouse_vals_to_change.push_back({button, true});
            return true;
        }
        return false;
    }

    static bool get_mouse(int button) {
        return mouse_pressed[button];
    }

    static bool get_mouse_up(int button) {
        if (mouse_released[button]) {
            if (!contains_key(mouse_vals_to_change, button))
                keys_to_change.push_back({button, false});
            return true;
        }
        return false;
    }

    static Vec2 get_mouse_pos() {
        return mouse_pos;
    }

    static Vec2 get_prev_mouse_pos() {
        return prev_mouse_pos;
    }

    static Vec2 get_mouse_change() {
        return mouse_pos - prev_mouse_pos;
    }

    static void update() {
        last_key_pressed = Key::NONE;
        scroll_amt = 0;

        for (int i = 0; i < keys_to_change.size(); i++) {
            int key = keys_to_change[i].first;
            if (keys_to_change[i].second) // if first being pressed
                first_pressed_keys[key] = false;
            else
                released_keys[key] = false;
        }
        keys_to_change.clear();

        for (int i = 0; i < mouse_vals_to_change.size(); i++) {
            int button = mouse_vals_to_change[i].first;
            if (mouse_vals_to_change[i].second) // if first being pressed
                mouse_first_pressed[button] = false;
            else
                mouse_released[button] = false;
        }
        mouse_vals_to_change.clear();
        prev_mouse_pos = mouse_pos;
    }

private:
    static bool first_pressed_keys[NUM_KEYS];
    static bool pressed_keys[NUM_KEYS];
    static bool released_keys[NUM_KEYS];

    static bool mouse_first_pressed[8];
    static bool mouse_pressed[8];
    static bool mouse_released[8];

    static Vec2 mouse_pos, prev_mouse_pos;

    static std::vector<std::pair<int, bool>> keys_to_change;
    static std::vector<std::pair<int, bool>> mouse_vals_to_change;

    static int scroll_amt;
    static Key last_key_pressed;

    static bool contains_key(const std::vector<std::pair<int, bool>>& v, int key) {
        for (int i = 0; i < v.size(); i++) {
            if (v[i].first == key)
                return true;
        }
        return false;
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        first_pressed_keys[key] = (action == GLFW_PRESS);
        pressed_keys[key] = (action != GLFW_RELEASE);
        released_keys[key] = (action == GLFW_RELEASE);
        if (action == GLFW_PRESS)
            last_key_pressed = (Key)key;
    }
    static void mouse_move_callback(GLFWwindow* window, double xpos, double ypos) {
        mouse_pos = Vec(xpos, ypos);
    }
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        bool pressed = (action == GLFW_PRESS);
        if (!mouse_pressed[button])
            mouse_first_pressed[button] = pressed;
        mouse_pressed[button] = pressed;
        mouse_released[button] = !pressed;
    }
};

// Declare static variables
bool Input::first_pressed_keys[NUM_KEYS];
bool Input::pressed_keys[NUM_KEYS];
bool Input::released_keys[NUM_KEYS];
bool Input::mouse_first_pressed[8];
bool Input::mouse_pressed[8];
bool Input::mouse_released[8];
Vec2 Input::mouse_pos, Input::prev_mouse_pos;

std::vector<std::pair<int, bool>> Input::keys_to_change;
std::vector<std::pair<int, bool>> Input::mouse_vals_to_change;

int Input::scroll_amt;
Key Input::last_key_pressed;

#endif