#include "camera.hxx"
#include "engine.hxx"
#include "hero.hxx"
#include "map.hxx"

#include <chrono>
#include <cstdlib>
#include <memory>

#include <glm/gtc/type_ptr.hpp>

int main(int /*argc*/, char** /*argv*/)
{
    std::unique_ptr<engine, void (*)(engine*)> engine(create_engine(),
                                                      destroy_engine);

    if (!engine->initialize())
        return EXIT_FAILURE;

    float window_width  = static_cast<float>(engine->get_window_width());
    float window_height = static_cast<float>(engine->get_window_height());

    camera* camera = create_camera(window_width, window_height);

    // Warrior creating

    texture* warrior_idle_sprite_sheet = create_texture();
    warrior_idle_sprite_sheet->load("./img/warrior_idle.png");
    texture* warrior_run_sprite_sheet = create_texture();
    warrior_run_sprite_sheet->load("./img/warrior_run.png");
    texture* warrior_attack_sprite_sheet = create_texture();
    warrior_attack_sprite_sheet->load("./img/warrior_attack.png");
    texture* warrior_run_n_attack_sprite_sheet = create_texture();
    warrior_run_n_attack_sprite_sheet->load("./img/warrior_run&attack.png");

    sprite* warrior_idle =
        create_sprite(warrior_idle_sprite_sheet, 48.f, 48.f, 6, 24.f);
    sprite* warrior_run =
        create_sprite(warrior_run_sprite_sheet, 48.f, 48.f, 6, 24.f);
    sprite* warrior_attack =
        create_sprite(warrior_attack_sprite_sheet, 86.f, 48.f, 4, 6.f);
    sprite* warrior_run_n_attack =
        create_sprite(warrior_run_n_attack_sprite_sheet, 64.f, 48.f, 4, 16.f);

    game_object* warrior = create_hero(
        window_width / 2.f, window_height / 2.f, 2.f, game_object_state::idle);
    warrior->add_sprite(warrior_idle, game_object_state::idle);
    warrior->add_sprite(warrior_run, game_object_state::run);
    warrior->add_sprite(warrior_attack, game_object_state::attack);
    warrior->add_sprite(warrior_run_n_attack, game_object_state::run_n_attack);

    index_buffer* warrior_index_buffer = create_index_buffer();
    warrior_index_buffer->add_indexes(4);

    glm::mat4 warrior_mat_size{ 1 };
    warrior_mat_size[0].x = warrior_mat_size[1].y = warrior->get_size();

    // Skeleton creating

    texture* skeleton_run_sprite_sheet = create_texture();
    skeleton_run_sprite_sheet->load("./img/skeleton_run.png");

    sprite* skeleton_run =
        create_sprite(skeleton_run_sprite_sheet, 74.f, 64.f, 8, 32.f);

    game_object* skeleton = create_hero(
        window_width, window_height / 2.f, 1.8f, game_object_state::run);
    skeleton->add_sprite(skeleton_run, game_object_state::run);
    skeleton->set_direction(0);

    index_buffer* skeleton_index_buffer = create_index_buffer();
    skeleton_index_buffer->add_indexes(4);

    glm::mat4 skeleton_mat_size{ 1 };
    skeleton_mat_size[0].x = skeleton_mat_size[1].y = skeleton->get_size();

    // Map creating

    texture* floor = create_texture();
    floor->load("./img/floor.png");
    texture* wall = create_texture();
    wall->load("./img/wall.png");
    texture* wall_top = create_texture();
    wall_top->load("./img/wall_top.png");
    texture* wall_left = create_texture();
    wall_left->load("./img/wall_left.png");
    texture* wall_right = create_texture();
    wall_right->load("./img/wall_right.png");
    texture* wall_bottom = create_texture();
    wall_bottom->load("./img/wall_bottom.png");

    map* dungeon_map = create_map(64.f, 64.f);
    dungeon_map->add_tile(floor, map_tile::floor);
    dungeon_map->add_tile(wall, map_tile::wall);
    dungeon_map->add_tile(wall_top, map_tile::wall_top);
    dungeon_map->add_tile(wall_left, map_tile::wall_left);
    dungeon_map->add_tile(wall_right, map_tile::wall_right);
    dungeon_map->add_tile(wall_bottom, map_tile::wall_bottom);
    generate_map(dungeon_map);

    // vertex and indexes to map

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
        engine->create_sound_buffer("./sound/dungeon_music.wav");
    music->play(audio_properties::looped);
    sound_buffer* sound_attack =
        engine->create_sound_buffer("./sound/attack.wav");

    float last_time        = engine->get_time();
    bool  quit             = false;
    bool  show_menu_window = true;
    event event;

    while (!quit)
    {
        if (engine->read_input(event))
        {
            if (event == event::turn_off)
            {
                quit = true;
                break;
            }

            if (event == event::pressed)
            {
                if (engine->check_action(action::up))
                {
                    warrior->set_state(game_object_state::run);
                    warrior->move(0.f, -20.f);
                    if (dungeon_map->check_collision(warrior,
                                                     map_tile::wall_top,
                                                     window_width,
                                                     window_height))
                        warrior->move(0.f, 20.f);
                }
                else if (engine->check_action(action::down))
                {
                    warrior->set_state(game_object_state::run);
                    warrior->move(0.f, 20.f);
                    if (dungeon_map->check_collision(warrior,
                                                     map_tile::wall_bottom,
                                                     window_width,
                                                     window_height))
                        warrior->move(0.f, -20.f);
                }
                else if (engine->check_action(action::left))
                {
                    warrior->set_state(game_object_state::run);
                    warrior->move(-20.f, 0.f);
                    if (dungeon_map->check_collision(warrior,
                                                     map_tile::wall_left,
                                                     window_width,
                                                     window_height))
                        warrior->move(20.f, 0.f);
                    warrior->set_direction(1);
                }
                else if (engine->check_action(action::right))
                {
                    warrior->set_state(game_object_state::run);
                    warrior->move(20.f, 0.f);
                    if (dungeon_map->check_collision(warrior,
                                                     map_tile::wall_right,
                                                     window_width,
                                                     window_height))
                        warrior->move(-20.f, 0.f);
                    warrior->set_direction(0);
                }

                if (engine->check_action(action::attack))
                {
                    if (warrior->get_state() == game_object_state::run)
                        warrior->set_state(game_object_state::run_n_attack);
                    else
                        warrior->set_state(game_object_state::attack);

                    if (warrior->get_sprite()->get_current_number() == 2)
                        sound_attack->play(audio_properties::once);
                }
            }
            else
                warrior->set_state(game_object_state::idle);
        }

        camera->look_at(warrior->get_current_pos_x(),
                        warrior->get_current_pos_y());

        float*    mat_view_first_value = camera->get_view();
        glm::mat4 mat_view             = glm::make_mat4x4(mat_view_first_value);

        if (show_menu_window)
        {
            if (engine->render_menu(show_menu_window))
                quit = true;
        }
        else
        {
            engine->render(floor_vertex_buffer,
                           floor_index_buffer,
                           dungeon_map->get_tile(map_tile::floor),
                           camera->get_view());

            engine->render(wall_vertex_buffer,
                           wall_index_buffer,
                           dungeon_map->get_tile(map_tile::wall),
                           camera->get_view());

            engine->render(wall_top_vertex_buffer,
                           wall_top_index_buffer,
                           dungeon_map->get_tile(map_tile::wall_top),
                           camera->get_view());

            engine->render(wall_bottom_vertex_buffer,
                           wall_bottom_index_buffer,
                           dungeon_map->get_tile(map_tile::wall_bottom),
                           camera->get_view());

            engine->render(wall_left_vertex_buffer,
                           wall_left_index_buffer,
                           dungeon_map->get_tile(map_tile::wall_left),
                           camera->get_view());

            engine->render(wall_right_vertex_buffer,
                           wall_right_index_buffer,
                           dungeon_map->get_tile(map_tile::wall_right),
                           camera->get_view());

            engine->render(warrior->get_vertex_buffer(),
                           warrior_index_buffer,
                           warrior->get_sprite(),
                           warrior->get_direction(),
                           &warrior_mat_size[0][0]);

            engine->render(skeleton->get_vertex_buffer(),
                           skeleton_index_buffer,
                           skeleton->get_sprite(),
                           skeleton->get_direction(),
                           &(mat_view * skeleton_mat_size)[0][0]);

            if (engine->get_time() - last_time > 0.15f)
            {
                warrior->get_sprite()->next_sprite();
                skeleton->get_sprite()->next_sprite();
                last_time = engine->get_time();
            }
        }

        if (!engine->swap_buffers())
        {
            engine->uninitialize();
            return EXIT_FAILURE;
        }
    }

    warrior_idle_sprite_sheet->delete_texture();
    delete warrior_idle_sprite_sheet;
    engine->uninitialize();

    return EXIT_SUCCESS;
}
