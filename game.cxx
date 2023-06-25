#include "camera.hxx"
#include "engine.hxx"
#include "map.hxx"
#include "sprite.hxx"

#include <chrono>
#include <cstdlib>
#include <memory>

int main(int /*argc*/, char** /*argv*/)
{
    std::unique_ptr<engine, void (*)(engine*)> engine(create_engine(),
                                                      destroy_engine);

    if (!engine->initialize())
        return EXIT_FAILURE;

    camera* camera = create_camera();

    texture* warrior_idle_sprite_sheet = create_texture();
    warrior_idle_sprite_sheet->load("./img/warrior_idle.png", 6);
    texture* warrior_run_sprite_sheet = create_texture();
    warrior_run_sprite_sheet->load("./img/warrior_run.png", 6);
    texture* warrior_attack_sprite_sheet = create_texture();
    warrior_attack_sprite_sheet->load("./img/warrior_attack.png", 4);
    texture* warrior_run_n_attack_sprite_sheet = create_texture();
    warrior_run_n_attack_sprite_sheet->load("./img/warrior_run&attack.png", 4);

    sprite* warrior_sprite =
        create_sprite(static_cast<float>(engine->get_window_width()) / 2.f,
                      static_cast<float>(engine->get_window_height()) / 2.f,
                      192.f,
                      96.f);
    warrior_sprite->add_texture(warrior_idle_sprite_sheet);
    warrior_sprite->add_texture(warrior_run_sprite_sheet);
    warrior_sprite->add_texture(warrior_attack_sprite_sheet);
    warrior_sprite->add_texture(warrior_run_n_attack_sprite_sheet);

    vertex_buffer* warrior_vertex_buffer = create_vertex_buffer();
    warrior_vertex_buffer->buffer_data(warrior_sprite->get_vertices(),
                                       static_cast<size_t>(4));

    index_buffer* warrior_index_buffer = create_index_buffer();
    warrior_index_buffer->add_indexes(warrior_vertex_buffer->get_size());

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

    int direction = 0;

    float last_time        = engine->get_time();
    bool  quit             = false;
    bool  show_menu_window = true;
    event event;

    bool warrior_idle   = true;
    bool warrior_run    = false;
    bool warrior_attack = false;

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
                    warrior_idle = false;
                    warrior_run  = true;
                    warrior_sprite->move(0.f, 0.02f);
                    if (dungeon_map->check_collision(warrior_sprite,
                                                     map_tile::wall_top))
                        warrior_sprite->move(0.f, -0.02f);
                }
                else if (engine->check_action(action::down))
                {
                    warrior_idle = false;
                    warrior_run  = true;
                    warrior_sprite->move(0.f, -0.02f);
                    if (dungeon_map->check_collision(warrior_sprite,
                                                     map_tile::wall_bottom))
                        warrior_sprite->move(0.f, 0.02f);
                }
                else if (engine->check_action(action::left))
                {
                    warrior_idle = false;
                    warrior_run  = true;
                    warrior_sprite->move(-0.02f, 0.f);
                    if (dungeon_map->check_collision(warrior_sprite,
                                                     map_tile::wall_left))
                        warrior_sprite->move(0.02f, 0.f);
                    direction = 1;
                }
                else if (engine->check_action(action::right))
                {
                    warrior_idle = false;
                    warrior_run  = true;
                    warrior_sprite->move(0.02f, 0.f);
                    if (dungeon_map->check_collision(warrior_sprite,
                                                     map_tile::wall_right))
                        warrior_sprite->move(-0.02f, 0.f);
                    direction = 0;
                }

                if (engine->check_action(action::attack))
                {
                    warrior_attack = true;
                    sound_attack->play(audio_properties::once);
                }
                else
                    warrior_attack = false;
            }
            else
            {
                warrior_idle = true;
                warrior_run  = false;
            }
        }

        camera->look_at(warrior_sprite->get_current_pos_x(),
                        warrior_sprite->get_current_pos_y());

        if (show_menu_window)
        {
            if (engine->render_menu(show_menu_window))
                quit = true;
        }
        else
        {
            if (warrior_idle)
            {
                if (warrior_attack)
                    warrior_sprite->set_current_texture(2);
                else
                    warrior_sprite->set_current_texture(0);
            }
            else if (warrior_run)
            {
                if (warrior_attack)
                    warrior_sprite->set_current_texture(3);
                else
                    warrior_sprite->set_current_texture(1);
            }

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

            engine->render(warrior_vertex_buffer,
                           warrior_index_buffer,
                           warrior_sprite->get_sprite(),
                           direction);

            if (engine->get_time() - last_time > 0.15f)
            {
                warrior_sprite->next_sprite();
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
