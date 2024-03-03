#include "engine/include/buttons.hxx"
#include "engine/include/engine.hxx"

#include "include/camera.hxx"
#include "include/enemy.hxx"
#include "include/hero.hxx"
#include "include/interface.hxx"
#include "include/map.hxx"
#include "include/resources.hxx"

#include <cstdlib>
#include <memory>
#include <thread>

#ifdef __ANDROID__
#include <SDL3/SDL_main.h>
#endif

#include <glm/gtc/type_ptr.hpp>

int main(int /*argc*/, char** /*argv*/)
{
    if (!prip_engine::init())
        return EXIT_FAILURE;

    resources::init();

    float window_width =
        static_cast<float>(prip_engine::get_window_width_pixels());
    float window_height =
        static_cast<float>(prip_engine::get_window_height_pixels());

    glm::mat4 projection =
        glm::ortho<float>(0.f, window_width, window_height, 0.f, -1.f, 1.f);

    camera* camera = create_camera(window_width / 2, window_height / 2);

    using namespace std::chrono_literals;

    // Map creating
    map* dungeon_map = new map(64.f, 64.f, "data/level_1.txt");

    // Warrior creating
    hero* warrior =
        new hero({ 300.f, 464.f }, 10.f, 2.f, 0, dungeon_map, 4, 12.f, 300.f);

    // Skeleton creating

    // std::vector<enemy*> enemies;
    /*enemy* skel = new skeleton_spearman(
        { 800.f, 418.f }, 4, 4.f, 2.f, 2000ms, 400.f, 2000ms);*/
    /*enemy* skel = new skeleton_warrior({ 800.f, 450.f }, 4, 5.f, 2.f,
     * 2000ms);*/
    enemy* skel = new skeleton_archer(
        { 800, 450.f }, 4.f, 2.f, 1, dungeon_map, 4, 2000ms);

    // Interface

    std::vector<interface*> health;
    health.resize(2);

    health[0] = create_interface(10.f, 10.f, 64.f, 64.f);
    health[0]->add_texture(resources::heart_full);
    health[0]->add_texture(resources::heart_half);
    health[0]->add_texture(resources::heart_empty);

    health[1] = create_interface(84.f, 10.f, 64.f, 64.f);
    health[1]->add_texture(resources::heart_full);
    health[1]->add_texture(resources::heart_half);
    health[1]->add_texture(resources::heart_half);

    // Sound

    prip_engine::sound_buffer* music =
        prip_engine::create_sound_buffer("sound/dungeon_music.wav");
    music->play(prip_engine::audio_properties::looped);
    /*prip_engine::sound_buffer* sound_attack =
        prip_engine::create_sound_buffer("sound/attack.wav");*/

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
                warrior->update(frame_time_dif, skel);
                skel->update(frame_time_dif, warrior);
            }

            camera->look_at(warrior->get_global_pos());

            // Render
            glm::mat4 view            = glm::make_mat4x4(camera->get_view());
            glm::mat4 projection_view = projection * view;

            dungeon_map->draw(&projection_view[0][0]);
            skel->draw(&projection_view[0][0]);
            warrior->draw(&projection_view[0][0]);

            switch (warrior->get_health())
            {
                case 0:
                    health[0]->set_texture(2);
                case 1:
                    health[0]->set_texture(1);
                    break;
                case 2:
                    health[0]->set_texture(0);
                    health[1]->set_texture(2);
                    break;
                case 3:
                    health[1]->set_texture(1);
                    break;
                case 4:
                    health[1]->set_texture(0);
                    break;
                default:
                    health[1]->set_texture(0);
            }

            /*for (auto heart : health)
                prip_engine->render(heart->get_tileset(),
                               heart->get_vertex_buffer(),
                               solo_objects_index_buffer,
                               &projection[0][0]);*/

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
