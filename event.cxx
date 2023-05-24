#include "event.hxx"

#include <algorithm>
#include <array>

#include <SDL3/SDL_events.h>

static std::array<std::string_view, 17> event_names = {
    { "Up button pressed",
      "Up button released",
      "Down button pressed",
      "Down button released",
      "Left button pressed",
      "Left button released",
      "Right button pressed",
      "Right button released",
      "Button1 pressed",
      "Button1 released",
      "Button2 pressed",
      "Button2 released",
      "Select button pressed",
      "Select button released",
      "Start button pressed",
      "Start button released",
      "Turn off" }
};

std::ostream& operator<<(std::ostream& out, const event& event)
{
    int min_value = static_cast<int>(event::up_pressed);
    int max_value = static_cast<int>(event::null_event);
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
    event            event_pressed;
    event            event_released;
};

std::array<bind, 9> key_bindings = {
    { { SDLK_w, "Up", event::up_pressed, event::up_released },
      { SDLK_s, "Down", event::down_pressed, event::down_released },
      { SDLK_a, "Left", event::left_pressed, event::left_released },
      { SDLK_d, "Right", event::right_pressed, event::right_released },
      { SDLK_1, "Button 1", event::button1_pressed, event::button1_released },
      { SDLK_2, "Button 2", event::button2_pressed, event::button2_released },
      { SDLK_e, "Select", event::select_pressed, event::select_released },
      { SDLK_SPACE, "Start", event::start_pressed, event::start_released },
      { SDLK_ESCAPE, "Close", event::turn_off, event::null_event } }
};

bool check_pressing_key(SDL_Event sdl_event, event& event)
{
    const auto it = std::find_if(key_bindings.begin(),
                                 key_bindings.end(),
                                 [&](const ::bind& b)
                                 { return b.key == sdl_event.key.keysym.sym; });
    if (it != key_bindings.end())
    {
        if (sdl_event.type == SDL_EVENT_KEY_DOWN)
            event = it->event_pressed;
        else if (sdl_event.type == SDL_EVENT_KEY_UP)
            event = it->event_released;
        return true;
    }
    return false;
}
