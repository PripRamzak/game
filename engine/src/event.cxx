#include "include/event.hxx"
#include "include/buttons.hxx"

#include <algorithm>
#include <array>
#include <vector>

#include <SDL3/SDL_events.h>

namespace prip_engine
{
static std::array<std::string_view, 3> event_names = {
    { "Pressed", "Released", "Turn off" }
};

std::ostream& operator<<(std::ostream& out, const event& event)
{
    int min_value = static_cast<int>(event::pressed);
    int max_value = static_cast<int>(event::turn_off);
    int cur_value = static_cast<int>(event);
    if (cur_value >= min_value && cur_value <= max_value)
    {
        out << event_names[cur_value];
        return out;
    }
    else
        throw std::runtime_error("There is no such event value");
}

struct bind
{
    SDL_Keycode      keycode;
    std::string_view name;
    key              key_;
    event            event_;
};

std::array<bind, 7> key_bindings = {
    { { SDLK_W, "Up", key::up, event::released },
      { SDLK_S, "Down", key::down, event::released },
      { SDLK_A, "Left", key::left, event::released },
      { SDLK_D, "Right", key::right, event::released },
      { SDLK_SPACE, "Jump", key::jump, event::released },
      { SDLK_J, "Attack", key::attack, event::released },
      { SDLK_ESCAPE, "Menu", key::menu, event::released } }
};

bool is_key_down(key key)
{
    const auto it = std::find_if(begin(key_bindings),
                                 key_bindings.end(),
                                 [&](const bind& b) { return b.key_ == key; });

    if (it != key_bindings.end())
    {
        const bool* state = SDL_GetKeyboardState(nullptr);

        int sdl_scan_code = SDL_GetScancodeFromKey(it->keycode, SDL_KMOD_NONE);
        return state[sdl_scan_code];
    }

    return false;
}

bool check_pressing_key(SDL_Event sdl_event, event& event)
{
    const auto it = std::find_if(key_bindings.begin(),
                                 key_bindings.end(),
                                 [&](const bind& b)
                                 { return b.keycode == sdl_event.key.key; });

    if (it != key_bindings.end())
    {
        if (sdl_event.type == SDL_EVENT_KEY_DOWN)
        {
            event      = event::pressed;
            it->event_ = event::pressed;
        }
        else if (sdl_event.type == SDL_EVENT_KEY_UP)
        {
            event      = event::released;
            it->event_ = event::released;
        }
        return true;
    }

    return false;
}

/*
bool check_pressing_button(SDL_Event             sdl_event,
                           event&                event,
                           std::vector<buttons>& buttons_,
                           float                 window_width,
                           float                 window_height)
{
    float finger_x = sdl_event.tfinger.x * window_width;
    float finger_y = sdl_event.tfinger.y * window_height;

    vertex2d_uv movement_buttons_vertex = *buttons_[0].vertex_buffer_->data();
    vertex2d_uv attack_button_vertex    = *buttons_[1].vertex_buffer_->data();

    if (finger_x > movement_buttons_vertex.pos.x + buttons_[0].width / 3 &&
        finger_x < movement_buttons_vertex.pos.x + buttons_[0].width / 3 * 2 &&
        finger_y > movement_buttons_vertex.pos.y &&
        finger_y < movement_buttons_vertex.pos.y + buttons_[0].height / 3)
    {
        if (sdl_event.type == SDL_EVENT_FINGER_DOWN)
        {
            event                  = event::pressed;
            key_bindings[0].event_ = event::pressed;
        }
        else
        {
            event                  = event::released;
            key_bindings[0].event_ = event::released;
        }
        return true;
    }
    else if (finger_x > movement_buttons_vertex.pos.x + buttons_[0].width / 3 &&
             finger_x <
                 movement_buttons_vertex.pos.x + buttons_[0].width / 3 * 2 &&
             finger_y >
                 movement_buttons_vertex.pos.y + buttons_[0].height / 3 * 2 &&
             finger_y < movement_buttons_vertex.pos.y + buttons_[0].height)
    {
        if (sdl_event.type == SDL_EVENT_FINGER_DOWN)
        {
            event                  = event::pressed;
            key_bindings[1].event_ = event::pressed;
        }
        else
        {
            event                  = event::released;
            key_bindings[1].event_ = event::released;
        }
        return true;
    }
    else if (finger_x > movement_buttons_vertex.pos.x &&
             finger_x < movement_buttons_vertex.pos.x + buttons_[0].width / 3 &&
             finger_y >
                 movement_buttons_vertex.pos.y + buttons_[0].height / 3 &&
             finger_y <
                 movement_buttons_vertex.pos.y + buttons_[0].height / 3 * 2)
    {
        if (sdl_event.type == SDL_EVENT_FINGER_DOWN)
        {
            event                  = event::pressed;
            key_bindings[2].event_ = event::pressed;
        }
        else
        {
            event                  = event::released;
            key_bindings[2].event_ = event::released;
        }
        return true;
    }
    else if (finger_x > movement_buttons_vertex.pos.x + buttons_[0].width / 3 &&
             finger_x < movement_buttons_vertex.pos.x + buttons_[0].width &&
             finger_y >
                 movement_buttons_vertex.pos.y + buttons_[0].height / 3 &&
             finger_y <
                 movement_buttons_vertex.pos.y + buttons_[0].height / 3 * 2)
    {
        if (sdl_event.type == SDL_EVENT_FINGER_DOWN)
        {
            event                  = event::pressed;
            key_bindings[3].event_ = event::pressed;
        }
        else
        {
            event                  = event::released;
            key_bindings[3].event_ = event::released;
        }
        return true;
    }

    if (finger_x > attack_button_vertex.pos.x &&
        finger_x < attack_button_vertex.pos.x + buttons_[1].width &&
        finger_y > attack_button_vertex.pos.y &&
        finger_y < attack_button_vertex.pos.y + buttons_[1].height)
    {
        if (sdl_event.type == SDL_EVENT_FINGER_DOWN)
        {
            event                  = event::pressed;
            key_bindings[4].event_ = event::pressed;
        }
        else
        {
            event                  = event::released;
            key_bindings[4].event_ = event::released;
        }
        return true;
    }

    return false;
}*/
} // namespace prip_engine