#include "camera.hxx"
#include "enemy.hxx"
#include "engine.hxx"
#include "hero.hxx"
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

    index_buffer* game_objects_index_buffer = create_index_buffer();
    game_objects_index_buffer->add_indexes(static_cast<size_t>(4));

    // Warrior creating

    hero::initialize();
    hero* warrior = create_hero(4,
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

    // TODO vertex and indexes to map

    vertex_buffer* floor_vertex_buffer = create_vertex_buffer();
    dungeon_map->create_tile_vertex_buffer(floor_vertex_buffer,
                                           map_tile::floor);

    index_buffer* floor_index_buffer = create_index_buffer();
    floor_index_buffer->add_indexes(floor_vertex_buffer->get_size());

    vertex_buffer* wall_vertex_buffer = create_vertex_buffer();
    dungeon_map->create_tile_vertex_buffer(wall_vertex_buffer, map_tile::wall);

    index_buffer* wall_index_buffer = create_index_buffer();
    wall_index_buffer->add_indexes(wall_vertex_buffer->get_size());

    vertex_buffer* wall_top_vertex_buffer = create_vertex_buffer();
    dungeon_map->create_tile_vertex_buffer(wall_top_vertex_buffer,
                                           map_tile::wall_top);

    index_buffer* wall_top_index_buffer = create_index_buffer();
    wall_top_index_buffer->add_indexes(wall_top_vertex_buffer->get_size());

    vertex_buffer* wall_left_vertex_buffer = create_vertex_buffer();
    dungeon_map->create_tile_vertex_buffer(wall_left_vertex_buffer,
                                           map_tile::wall_left);

    index_buffer* wall_left_index_buffer = create_index_buffer();
    wall_left_index_buffer->add_indexes(wall_left_vertex_buffer->get_size());

    vertex_buffer* wall_right_vertex_buffer = create_vertex_buffer();
    dungeon_map->create_tile_vertex_buffer(wall_right_vertex_buffer,
                                           map_tile::wall_right);

    index_buffer* wall_right_index_buffer = create_index_buffer();
    wall_right_index_buffer->add_indexes(wall_right_vertex_buffer->get_size());

    vertex_buffer* wall_bottom_vertex_buffer = create_vertex_buffer();
    dungeon_map->create_tile_vertex_buffer(wall_bottom_vertex_buffer,
                                           map_tile::wall_bottom);

    index_buffer* wall_bottom_index_buffer = create_index_buffer();
    wall_bottom_index_buffer->add_indexes(
        wall_bottom_vertex_buffer->get_size());

    sound_buffer* music =
        engine->create_sound_buffer("sound/dungeon_music.wav");
    music->play(audio_properties::looped);
    sound_buffer* sound_attack =
        engine->create_sound_buffer("sound/attack.wav");

    float warrior_update_time  = engine->get_time();
    float skeleton_update_time = engine->get_time();
    bool  quit                 = false;
    bool  show_menu_window     = true;
    event event;

    while (!quit && warrior->is_alive())
    {
        if (engine->read_input(event))
        {
            if (event == event::turn_off)
            {
                quit = true;
                break;
            }

            if (event == event::pressed /*&& !show_menu_window*/)
            {
                if (engine->check_action(action::up))
                {
                    warrior->move(0.f, -20.f);
                    if (dungeon_map->check_hero_collision(warrior,
                                                          map_tile::wall_top))
                        warrior->move(0.f, 20.f);
                }
                else if (engine->check_action(action::down))
                {
                    warrior->move(0.f, 20.f);
                    if (dungeon_map->check_hero_collision(
                            warrior, map_tile::wall_bottom))
                        warrior->move(0.f, -20.f);
                }
                else if (engine->check_action(action::left))
                {
                    warrior->move(-20.f, 0.f);
                    warrior->set_direction(1);
                    if (dungeon_map->check_hero_collision(warrior,
                                                          map_tile::wall_left))
                        warrior->move(20.f, 0.f);
                }
                else if (engine->check_action(action::right))
                {
                    warrior->move(20.f, 0.f);
                    warrior->set_direction(0);
                    if (dungeon_map->check_hero_collision(warrior,
                                                          map_tile::wall_right))
                        warrior->move(-20.f, 0.f);
                }

                if (engine->check_action(action::attack))
                {
                    if (warrior->get_state() == game_object_state::run)
                        warrior->set_state(game_object_state::run_n_attack);
                    else
                        warrior->set_state(game_object_state::attack);

                    if (warrior->get_sprite()->get_current_number(
                            warrior->get_direction()) == 2)
                        sound_attack->play(audio_properties::once);
                }
            }
            else
                warrior->set_state(game_object_state::idle);
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

        engine->render(floor_vertex_buffer,
                       floor_index_buffer,
                       dungeon_map->get_tile(map_tile::floor),
                       &map_mat_result[0][0]);

        engine->render(wall_vertex_buffer,
                       wall_index_buffer,
                       dungeon_map->get_tile(map_tile::wall),
                       &map_mat_result[0][0]);

        engine->render(wall_top_vertex_buffer,
                       wall_top_index_buffer,
                       dungeon_map->get_tile(map_tile::wall_top),
                       &map_mat_result[0][0]);

        engine->render(wall_bottom_vertex_buffer,
                       wall_bottom_index_buffer,
                       dungeon_map->get_tile(map_tile::wall_bottom),
                       &map_mat_result[0][0]);

        engine->render(wall_left_vertex_buffer,
                       wall_left_index_buffer,
                       dungeon_map->get_tile(map_tile::wall_left),
                       &map_mat_result[0][0]);

        engine->render(wall_right_vertex_buffer,
                       wall_right_index_buffer,
                       dungeon_map->get_tile(map_tile::wall_right),
                       &map_mat_result[0][0]);

        // Objects render

        for (auto enemy : enemies)
            if (enemy->is_alive())
            {
                if (enemy->get_state() != game_object_state::attack)
                    enemy->move(warrior);
                else
                    enemy->attack(warrior);

                glm::mat4 skeleton_mat_size{ 1 };
                skeleton_mat_size[0].x = skeleton_mat_size[1].y =
                    enemy->get_size();

                glm::mat4 skeleton_mat_move{ 1 };
                skeleton_mat_move[3].x =
                    enemy->get_move_x() / window_width * 2.f;
                skeleton_mat_move[3].y =
                    enemy->get_move_y() / window_height * -2.f;

                glm::mat4 skeleton_mat_result = mat_view * skeleton_mat_move *
                                                skeleton_mat_size *
                                                to_ndc_coordinates;

                engine->render(enemy->get_vertex_buffer(),
                               game_objects_index_buffer,
                               enemy->get_sprite(),
                               enemy->get_direction(),
                               &skeleton_mat_result[0][0]);

                if (enemy->get_state() != game_object_state::idle &&
                    engine->get_time() - skeleton_update_time > 0.3f)
                {
                    enemy->get_sprite()->next_sprite();
                    skeleton_update_time = engine->get_time();
                }
                else if (enemy->get_state() == game_object_state::idle &&
                         engine->get_time() - skeleton_update_time > 1.f)
                {
                    enemy->set_state(game_object_state::attack);
                    skeleton_update_time = engine->get_time();
                }
            }

        glm::mat4 warrior_mat_result = warrior_mat_size * to_ndc_coordinates;

        engine->render(warrior->get_vertex_buffer(),
                       game_objects_index_buffer,
                       warrior->get_sprite(),
                       warrior->get_direction(),
                       &warrior_mat_result[0][0]);

        if (engine->get_time() - warrior_update_time > 0.15f)
        {
            warrior->get_sprite()->next_sprite();
            warrior_update_time = engine->get_time();
        }

        game_logic_level_1(warrior, enemies);
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
