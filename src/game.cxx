#include "engine/include/buttons.hxx"
#include "engine/include/camera.hxx"
#include "engine/include/engine.hxx"

#include "include/enemy.hxx"
#include "include/hero.hxx"
#include "include/interface.hxx"
#include "include/map.hxx"
#include "include/resources.hxx"

#include <cstdlib>
#include <memory>
#include <thread>

#include <variant>

#ifdef __ANDROID__
#include <SDL3/SDL_main.h>
#endif

int main(int /*argc*/, char** /*argv*/)
{
    if (!prip_engine::init())
        return EXIT_FAILURE;

    resources::init();

    // Map creating
    map* dungeon_map = new map("data/level1.tmx", 64);

    // Warrior creating
    hero* warrior = new hero({ 300.f, 256.f }, 0, dungeon_map);

    // Skeleton creating

    std::vector<enemy*> enemies;
    // enemies.push_back(
    //     new skeleton_spearman({ 800.f, 418.f }, 0, dungeon_map, warrior));
    // enemies.push_back(
    //     new skeleton_warrior({ 1700.f, 450.f }, 0, dungeon_map, warrior));
    // enemies.push_back(
    //     new skeleton_archer({ 2500, 450.f }, 1, dungeon_map, warrior));

    // Sound

    resources::main_theme->play(prip_engine::audio_properties::looped);

    bool               quit                     = false;
    bool               show_menu_window         = true;
    bool               show_in_game_menu_window = false;
    prip_engine::event event                    = prip_engine::event::released;

    prip_engine::timepoint start = prip_engine::get_time();

    while (!quit && warrior->is_alive())
    {
        // Event handling

        while (prip_engine::read_input(event))
            if (event == prip_engine::event::turn_off)
            {
                quit = true;
                break;
            }

        prip_engine::timepoint last_frame_time = prip_engine::get_time();

        std::chrono::milliseconds frame_time_dif =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                last_frame_time - start);

        if (frame_time_dif.count() < 1000 / 60)
        {
            std::this_thread::yield();
            continue;
        }
        start = last_frame_time;

        if (show_menu_window)
        {
            if (prip_engine::render_gui(show_menu_window,
                                        prip_engine::gui_type::menu))
                quit = true;
        }
        else
        {
            if (prip_engine::is_key_down(prip_engine::key::menu))
                show_in_game_menu_window = true;

            if (!show_in_game_menu_window)
            {
                warrior->update(frame_time_dif);
                for (auto enemy : enemies)
                    enemy->update(frame_time_dif);
            }

            // Render
            prip_engine::camera::look_at(warrior->get_global_pos());

            dungeon_map->draw();
            for (auto enemy : enemies)
                enemy->draw();
            warrior->draw();

            if (!show_in_game_menu_window)
            {
#ifdef __ANDROID__
                prip_engine->render_buttons(&to_ndc_coordinates[0][0]);
#endif
            }
            else if (prip_engine::render_gui(show_in_game_menu_window,
                                             prip_engine::gui_type::menu))
                quit = true;
        }

        if (!prip_engine::swap_buffers())
        {
            prip_engine::destroy();
            return EXIT_FAILURE;
        }
    }

    prip_engine::destroy();

    return EXIT_SUCCESS;
}
