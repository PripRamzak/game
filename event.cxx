#include "event.hxx"

#include <algorithm>
#include <array>

#include <SDL3/SDL_events.h>

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
    SDL_Keycode      key;
    std::string_view name;
    action           action_;
};

std::array<bind, 6> key_bindings = {
    { { SDLK_w, "Up", action::up },
      { SDLK_s, "Down", action::down },
      { SDLK_a, "Left", action::left },
      { SDLK_d, "Right", action::right },
      { SDLK_SPACE, "Attack", action::attack },
      { SDLK_ESCAPE, "Menu", action::menu } }
};

bool is_key_down(action action)
{
    const auto it =
        std::find_if(begin(key_bindings),
                     key_bindings.end(),
                     [&](const bind& b) { return b.action_ == action; });

    if (it != key_bindings.end())
    {
        const uint8_t* state         = SDL_GetKeyboardState(nullptr);
        int            sdl_scan_code = SDL_GetScancodeFromKey(it->key);
        return state[sdl_scan_code];
    }

    return false;
}

bool check_pressing_key(SDL_Event sdl_event, event& event)
{
    const auto it = std::find_if(key_bindings.begin(),
                                 key_bindings.end(),
                                 [&](const bind& b)
                                 { return b.key == sdl_event.key.keysym.sym; });

    if (it != key_bindings.end())
    {
        if (sdl_event.type == SDL_EVENT_KEY_DOWN)
            event = event::pressed;
        else if (sdl_event.type == SDL_EVENT_KEY_UP)
            event = event::released;
        return true;
    }

    return false;
}
