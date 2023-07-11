#include "buttons.hxx"
#include "camera.hxx"
#include "enemy.hxx"
#include "engine.hxx"
#include "hero.hxx"
#include "interface.hxx"
#include "map.hxx"

#include <chrono>
#include <cstdlib>
#include <memory>

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

    /*glm::mat4 projection =
        glm::ortho<float>(0.f, window_width, window_height, 0.f, -1.f, 1.f);*/

    glm::mat4 to_ndc_coordinates{ 1 };
    to_ndc_coordinates[0].x = 2.f / window_width;
    to_ndc_coordinates[1].y = -2.f / window_height;
    to_ndc_coordinates[3].x = -1.f;
    to_ndc_coordinates[3].y = 1.f;

    camera* camera = create_camera(window_width, window_height);

    index_buffer* solo_objects_index_buffer = create_index_buffer();
    solo_objects_index_buffer->add_indexes(static_cast<size_t>(4));

    // Warrior creating

    hero::initialize();
    hero* warrior = create_hero(4,
                                10.f,
                                window_width / 2.f,
                                window_height / 2.f,
                                window_width / 2.f,
                                window_height / 2.f,
                                2.f,
                                game_object_state::idle);

    glm::mat4 warrior_mat_size =
        glm::scale(glm::mat4{ 1 },
                   glm::vec3{ warrior->get_size(), warrior->get_size(), 1.f });

    // Skeleton creating

    enemy::initialize();
    std::vector<enemy*> enemies;

    // Map creating

    map::initialize();
    map* dungeon_map = create_map(64.f, 64.f);
    generate_level_1(dungeon_map, enemies, window_width, window_height);

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

    float warrior_update_time = engine->get_time();

    // 0 - top, 1 - bottom, 2 - left, 3 - right
    bool warrior_skeleton_collision[4]{ false };

    float skeleton_update_time = engine->get_time();

    bool  quit             = false;
    bool  show_menu_window = true;
    event event            = event::released;

    while (!quit && warrior->is_alive())
    {
        // Event handling

        engine->read_input(event);
        if (event == event::turn_off)
        {
            quit = true;
            break;
        }

        if (show_menu_window)
        {
            if (engine->render_menu(show_menu_window))
                quit = true;
        }
        else
        {
            camera->look_at(warrior->get_current_pos_x(),
                            warrior->get_current_pos_y());

            glm::mat4 mat_view = glm::make_mat4x4(camera->get_view());

            // Map render

            glm::mat4 map_mat_result = mat_view * to_ndc_coordinates;

            engine->render(dungeon_map->get_vertex_buffer(map_tile::floor),
                           dungeon_map->get_index_buffer(map_tile::floor),
                           dungeon_map->get_tile(map_tile::floor),
                           &map_mat_result[0][0]);

            engine->render(dungeon_map->get_vertex_buffer(map_tile::wall),
                           dungeon_map->get_index_buffer(map_tile::wall),
                           dungeon_map->get_tile(map_tile::wall),
                           &map_mat_result[0][0]);

            engine->render(dungeon_map->get_vertex_buffer(map_tile::wall_top),
                           dungeon_map->get_index_buffer(map_tile::wall_top),
                           dungeon_map->get_tile(map_tile::wall_top),
                           &map_mat_result[0][0]);

            engine->render(
                dungeon_map->get_vertex_buffer(map_tile::wall_bottom),
                dungeon_map->get_index_buffer(map_tile::wall_bottom),
                dungeon_map->get_tile(map_tile::wall_bottom),
                &map_mat_result[0][0]);

            engine->render(dungeon_map->get_vertex_buffer(map_tile::wall_left),
                           dungeon_map->get_index_buffer(map_tile::wall_left),
                           dungeon_map->get_tile(map_tile::wall_left),
                           &map_mat_result[0][0]);

            engine->render(dungeon_map->get_vertex_buffer(map_tile::wall_right),
                           dungeon_map->get_index_buffer(map_tile::wall_right),
                           dungeon_map->get_tile(map_tile::wall_right),
                           &map_mat_result[0][0]);

            // Objects render

            if (event == event::pressed)
            {
                if (engine->check_key(key::attack))
                {
                    warrior->set_state(game_object_state::attack);

                    if (warrior->get_sprite()->get_current_number(
                            warrior->get_direction()) == 2)
                        sound_attack->play(audio_properties::once);
                }
                else
                {
                    int dx = 0;
                    int dy = 0;

                    if (engine->check_key(key::up))
                        dy--;
                    if (engine->check_key(key::down))
                        dy++;
                    if (engine->check_key(key::left))
                        dx--;
                    if (engine->check_key(key::right))
                        dx++;
                    warrior->move(
                        dx, dy, dungeon_map, warrior_skeleton_collision);
                }
            }
            else
                warrior->set_state(game_object_state::idle);

            glm::mat4 warrior_mat_result =
                warrior_mat_size * to_ndc_coordinates;

            engine->render(warrior->get_vertex_buffer(),
                           solo_objects_index_buffer,
                           warrior->get_sprite(),
                           warrior->get_direction(),
                           &warrior_mat_result[0][0]);

            warrior_skeleton_collision[0] = false;
            warrior_skeleton_collision[1] = false;
            warrior_skeleton_collision[2] = false;
            warrior_skeleton_collision[3] = false;

            for (auto enemy : enemies)
            {
                if (enemy->is_alive())
                {
                    if (enemy->get_state() != game_object_state::attack)
                        enemy->move(warrior);
                    else
                        enemy->attack(warrior);

                    bool skeleton_warrior_collision =
                        enemy->check_hero_collision_x(warrior) &&
                        enemy->check_hero_collision_y(warrior);

                    if (skeleton_warrior_collision &&
                        warrior->get_current_pos_y() -
                                warrior->get_sprite()->get_height() / 2 *
                                    warrior->get_size() <
                            enemy->get_current_pos_y() +
                                enemy->get_sprite()->get_height() / 2 *
                                    enemy->get_size() &&
                        warrior->get_current_pos_y() -
                                warrior->get_sprite()->get_height() / 2 *
                                    warrior->get_size() >
                            enemy->get_current_pos_y() +
                                enemy->get_sprite()->get_height() / 2 *
                                    enemy->get_size() -
                                10.f)
                        warrior_skeleton_collision[0] = true;
                    if (skeleton_warrior_collision &&
                        warrior->get_current_pos_y() +
                                warrior->get_sprite()->get_height() / 2 *
                                    warrior->get_size() >
                            enemy->get_current_pos_y() -
                                enemy->get_sprite()->get_height() / 2 *
                                    enemy->get_size() &&
                        warrior->get_current_pos_y() +
                                warrior->get_sprite()->get_height() / 2 *
                                    warrior->get_size() <
                            enemy->get_current_pos_y() -
                                enemy->get_sprite()->get_height() / 2 *
                                    enemy->get_size() +
                                10.f)
                        warrior_skeleton_collision[1] = true;
                    if (skeleton_warrior_collision &&
                        warrior->get_current_pos_x() >
                            enemy->get_current_pos_x())
                        warrior_skeleton_collision[2] = true;
                    if (skeleton_warrior_collision &&
                        warrior->get_current_pos_x() <
                            enemy->get_current_pos_x())
                        warrior_skeleton_collision[3] = true;

                    if (warrior->get_state() == game_object_state::attack)
                        warrior->attack(enemy, skeleton_warrior_collision);

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

            for (auto heart : health)
                engine->render(heart->get_vertex_buffer(),
                               solo_objects_index_buffer,
                               heart->get_texture(),
                               &to_ndc_coordinates[0][0]);

            if (engine->get_time() - warrior_update_time >
                warrior->get_sprite()->get_animation_time())
            {
                warrior->get_sprite()->next_sprite();
                warrior_update_time = engine->get_time();
            }

#ifdef __ANDROID__
            engine->render_buttons(&to_ndc_coordinates[0][0]);
#endif

            game_logic_level_1(dungeon_map, warrior, enemies);
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
