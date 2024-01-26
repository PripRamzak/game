#include "engine/include/buttons.hxx"
#include "engine/include/engine.hxx"
#include "include/camera.hxx"
#include "include/enemy.hxx"
#include "include/hero.hxx"
#include "include/interface.hxx"
#include "include/map.hxx"
#include <include/enemy.hxx>

#include <chrono>
#include <cstdlib>
#include <memory>
#include <thread>

#ifdef __ANDROID__
#include <SDL3/SDL_main.h>
#endif

#include <glm/gtc/type_ptr.hpp>

int main(int /*argc*/, char** /*argv*/)
{
    std::unique_ptr<engine, void (*)(engine*)> engine(create_engine(),
                                                      destroy_engine);

    if (!engine->initialize())
        return EXIT_FAILURE;

    float window_width = static_cast<float>(engine->get_window_width_pixels());
    float window_height =
        static_cast<float>(engine->get_window_height_pixels());

    glm::mat4 projection =
        glm::ortho<float>(0.f, window_width, window_height, 0.f, -1.f, 1.f);

    camera* camera = create_camera(window_width / 2, window_height / 2);

    index_buffer* solo_objects_index_buffer = create_index_buffer();
    solo_objects_index_buffer->add_indexes(static_cast<size_t>(4));

    // Warrior creating

    hero::initialize();
    hero* warrior = new hero(
        4, 10.f, 300.f, 550.f, 2.f, game_object_state::idle, 12.f, 300.f);

    glm::mat4 warrior_mat_size =
        glm::scale(glm::mat4{ 1 },
                   glm::vec3{ warrior->get_size(), warrior->get_size(), 1.f });

    // Skeleton creating

    enemy::initialize();
    std::vector<enemy*> enemies;

    // Map creating

    map::initialize();
    map* dungeon_map = new map(64.f, 64.f, "data/level_1.txt");

    // Interface

    texture* heart_full_texture  = create_texture("img/heart_full.png");
    texture* heart_half_texture  = create_texture("img/heart_half.png");
    texture* heart_empty_texture = create_texture("img/heart_empty.png");

    std::vector<interface*> health;
    health.resize(2);

    health[0] = create_interface(10.f, 10.f, 64.f, 64.f);
    health[0]->add_texture(heart_full_texture);
    health[0]->add_texture(heart_half_texture);
    health[0]->add_texture(heart_empty_texture);

    health[1] = create_interface(84.f, 10.f, 64.f, 64.f);
    health[1]->add_texture(heart_full_texture);
    health[1]->add_texture(heart_half_texture);
    health[1]->add_texture(heart_empty_texture);

    // Sound

    sound_buffer* music =
        engine->create_sound_buffer("sound/dungeon_music.wav");
    music->play(audio_properties::looped);
    sound_buffer* sound_attack =
        engine->create_sound_buffer("sound/attack.wav");

    // 0 - top, 1 - bottom, 2 - left, 3 - right
    // bool warrior_skeleton_collision[4]{ false };

    bool  quit                     = false;
    bool  show_menu_window         = true;
    bool  show_in_game_menu_window = false;
    event event                    = event::released;

    timepoint start = engine->get_time();

    while (!quit && warrior->is_alive())
    {
        // Event handling

        while (engine->read_input(event))
            if (event == event::turn_off)
            {
                quit = true;
                break;
            }

        timepoint last_frame_time = engine->get_time();

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
            if (engine->render_gui(show_menu_window, gui_type::menu))
                quit = true;
        }
        else
        {
            if (engine->check_key(key::menu))
                show_in_game_menu_window = true;

            if (!show_in_game_menu_window)
            {
                if (engine->check_key(key::attack) &&
                    warrior->get_state() != game_object_state::jump &&
                    warrior->get_state() != game_object_state::fall)
                {
                    warrior->set_state(game_object_state::attack);

                    if (warrior->get_animated_sprite()->get_current_number(
                            warrior->get_direction()) == 2)
                        sound_attack->play(audio_properties::once);
                }
                else if (engine->check_key(key::jump) &&
                         warrior->get_state() != game_object_state::jump &&
                         warrior->get_state() != game_object_state::fall)
                    warrior->set_state(game_object_state::jump);
                else
                {
                    float dx = 0.f;
                    float dy = 0.f;

                    if (warrior->get_state() == game_object_state::jump)
                    {
                        dy--;
                        warrior->jump();
                    }
                    else if (warrior->get_state() == game_object_state::fall)
                        dy++;
                    else if (!warrior->check_collision_map_y_axis(
                                 dungeon_map, map_tile_type::brick_bottom))
                        dy++;

                    if (engine->check_key(key::left))
                        dx--;
                    if (engine->check_key(key::right))
                        dx++;

                    if (dx == 0 && dy == 0)
                        warrior->set_state(game_object_state::idle);
                    else
                        warrior->move(dx, dy, dungeon_map);
                }

                warrior->get_animated_sprite()->play(frame_time_dif);
            }

            camera->look_at(warrior->get_global_pos_x(),
                            warrior->get_global_pos_y());

            glm::mat4 mat_view = glm::make_mat4x4(camera->get_view());

            // Map render

            glm::mat4 map_mat_result = projection * mat_view;

            /*engine->render(dungeon_map->get_tileset(),
                           dungeon_map->get_vertex_buffer(map_tile_type::wall),
                           dungeon_map->get_index_buffer(map_tile_type::wall),
                           dungeon_map->get_tile_min_uv(map_tile_type::wall),
                           dungeon_map->get_tile_max_uv(map_tile_type::wall),
                           &map_mat_result[0][0]);*/
            engine->render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(map_tile_type::brick_top),
                dungeon_map->get_index_buffer(map_tile_type::brick_top),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_top),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_top),
                &map_mat_result[0][0]);
            engine->render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(map_tile_type::brick_bottom),
                dungeon_map->get_index_buffer(map_tile_type::brick_bottom),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_bottom),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_bottom),
                &map_mat_result[0][0]);
            engine->render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(map_tile_type::brick_left),
                dungeon_map->get_index_buffer(map_tile_type::brick_left),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_left),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_left),
                &map_mat_result[0][0]);
            engine->render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(map_tile_type::brick_right),
                dungeon_map->get_index_buffer(map_tile_type::brick_right),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_right),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_right),
                &map_mat_result[0][0]);
            engine->render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(map_tile_type::brick_top_left),
                dungeon_map->get_index_buffer(map_tile_type::brick_top_left),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_top_left),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_top_left),
                &map_mat_result[0][0]);
            engine->render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(map_tile_type::brick_top_right),
                dungeon_map->get_index_buffer(map_tile_type::brick_top_right),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_top_right),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_top_right),
                &map_mat_result[0][0]);
            engine->render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(
                    map_tile_type::brick_bottom_left),
                dungeon_map->get_index_buffer(map_tile_type::brick_bottom_left),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_bottom_left),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_bottom_left),
                &map_mat_result[0][0]);
            engine->render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(
                    map_tile_type::brick_bottom_right),
                dungeon_map->get_index_buffer(
                    map_tile_type::brick_bottom_right),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_bottom_right),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_bottom_right),
                &map_mat_result[0][0]);

            // Objects render

            glm::mat4 war_mat_move =
                glm::translate(glm::mat4{ 1 },
                               glm::vec3(warrior->get_global_pos_x(),
                                         warrior->get_global_pos_y(),
                                         0.f));

            glm::mat4 warrior_mat_result =
                projection * mat_view * war_mat_move * warrior_mat_size;

            engine->render(warrior->get_animated_sprite(),
                           solo_objects_index_buffer,
                           warrior->get_direction(),
                           &warrior_mat_result[0][0]);

            /*warrior_skeleton_collision[0] = false;
            warrior_skeleton_collision[1] = false;
            warrior_skeleton_collision[2] = false;
            warrior_skeleton_collision[3] = false;

            for (auto enemy : enemies)
            {
                if (enemy->is_alive())
                {
                    if (!show_in_game_menu_window)
                    {
                        if (enemy->get_state() != game_object_state::attack)
                            enemy->move(warrior);
                        else
                            enemy->attack(warrior);

                        bool skeleton_warrior_collision =
                            enemy->check_hero_collision_x(warrior) &&
                            enemy->check_hero_collision_y(warrior);

                        if (warrior->get_state() ==
            game_object_state::attack) warrior->attack(enemy,
            skeleton_warrior_collision);
                    }

                    glm::mat4 skeleton_mat_size{ 1 };
                    skeleton_mat_size[0].x = skeleton_mat_size[1].y =
                        enemy->get_size();

                    glm::mat4 skeleton_mat_move{ 1 };
                    skeleton_mat_move[3].x =
                        enemy->get_move_x() / window_width * 2.f;
                    skeleton_mat_move[3].y =
                        enemy->get_move_y() / window_height * -2.f;

                    glm::mat4 skeleton_mat_result =
                        mat_view * skeleton_mat_move * skeleton_mat_size *
                        to_ndc_coordinates;

                    engine->render(enemy->get_vertex_buffer(),
                                   solo_objects_index_buffer,
                                   enemy->get_sprite(),
                                   enemy->get_direction(),
                                   &skeleton_mat_result[0][0]);

                    if (!show_in_game_menu_window)
                    {
                        if (engine->get_time() - skeleton_update_time >
                            enemy->get_sprite()->get_animation_time())
                        {
                            enemy->get_sprite()->next_sprite();
                            skeleton_update_time = engine->get_time();
                        }

                        if (enemy->get_state() == game_object_state::idle &&
                            engine->get_time() - skeleton_update_time > 1.f)
                        {
                            enemy->set_state(game_object_state::attack);
                            skeleton_update_time = engine->get_time();
                        }
                    }
                }
            }*/

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
                engine->render(heart->get_tileset(),
                               heart->get_vertex_buffer(),
                               solo_objects_index_buffer,
                               &projection[0][0]);*/

            if (!show_in_game_menu_window)
            {
#ifdef __ANDROID__
                engine->render_buttons(&to_ndc_coordinates[0][0]);
#endif

                // game_logic_level_1(dungeon_map, warrior, enemies);
            }
            else if (engine->render_gui(show_in_game_menu_window,
                                        gui_type::menu))
                quit = true;
        }

        if (!engine->swap_buffers())
        {
            engine->uninitialize();
            return EXIT_FAILURE;
        }
    }

    engine->uninitialize();

    return EXIT_SUCCESS;
}
