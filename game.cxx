#include "engine.hxx"
#include "sprite.hxx"

#include <chrono>
#include <cstdlib>
#include <memory>

#include <glm/matrix.hpp>

int main(int /*argc*/, char** /*argv*/)
{
    std::unique_ptr<engine, void (*)(engine*)> engine(create_engine(),
                                                      destroy_engine);

    if (!engine->initialize())
        return EXIT_FAILURE;

    texture* warrior_idle_sprite_sheet = create_texture();
    warrior_idle_sprite_sheet->load("./img/warrior_idle.png", 6);
    texture* warrior_run_sprite_sheet = create_texture();
    warrior_run_sprite_sheet->load("./img/warrior_run.png", 6);
    texture* warrior_attack_sprite_sheet = create_texture();
    warrior_attack_sprite_sheet->load("./img/warrior_attack.png", 4);
    texture* warrior_run_n_attack_sprite_sheet = create_texture();
    warrior_run_n_attack_sprite_sheet->load("./img/warrior_run&attack.png", 4);

    sprite* warrior_sprite = create_sprite(-120.f, 120.f, 288.f, 144.f);
    warrior_sprite->add_texture(warrior_idle_sprite_sheet);
    warrior_sprite->add_texture(warrior_run_sprite_sheet);
    warrior_sprite->add_texture(warrior_attack_sprite_sheet);
    warrior_sprite->add_texture(warrior_run_n_attack_sprite_sheet);

    vertex_buffer* warrior_vertex_buffer = create_vertex_buffer();
    warrior_vertex_buffer->buffer_data(warrior_sprite->get_vertices(),
                                       static_cast<size_t>(4));
    std::vector<uint16_t> indexes              = { 0, 1, 2, 0, 2, 3 };
    index_buffer*         warrior_index_buffer = create_index_buffer();
    warrior_index_buffer->buffer_data(indexes, indexes.size());

    int direction = 0;

    glm::mat3 mat_to_ndc_coordinates{ 1.f };
    mat_to_ndc_coordinates[0].x =
        1.f / static_cast<float>(engine->get_window_width());
    mat_to_ndc_coordinates[1].y =
        1.f / static_cast<float>(engine->get_window_height());

    glm::mat3 mat_projection{ 1.f };
    mat_projection[1].y = static_cast<float>(engine->get_window_width()) /
                          engine->get_window_height();

    sound_buffer* music =
        engine->create_sound_buffer("./sound/dungeon_music.wav");
    music->play(audio_properties::looped);
    sound_buffer* sound_attack =
        engine->create_sound_buffer("./sound/attack.wav");

    float last_time        = engine->get_time();
    bool  quit             = false;
    bool  show_menu_window = true;
    event event;

    bool warrior_idle   = true;
    bool warrior_run    = false;
    bool warrior_attack = false;

    while (!quit)
    {
        glm::mat3 mat_move{ 1 };
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
                    warrior_sprite->move(0.f, 0.03f);
                }
                else if (engine->check_action(action::down))
                {
                    warrior_idle = false;
                    warrior_run  = true;
                    warrior_sprite->move(0.f, -0.03f);
                }
                else if (engine->check_action(action::left))
                {
                    warrior_idle = false;
                    warrior_run  = true;
                    warrior_sprite->move(-0.03f, 0.f);
                    direction = 1;
                }
                else if (engine->check_action(action::right))
                {
                    warrior_idle = false;
                    warrior_run  = true;
                    warrior_sprite->move(0.03f, 0.f);
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

        mat_move[2].x = warrior_sprite->get_current_pos_x();
        mat_move[2].y = warrior_sprite->get_current_pos_y();

        if (show_menu_window)
        {
            if (engine->render_menu(show_menu_window))
                quit = true;
        }
        else
        {
            glm::mat3 mat_result =
                mat_move * mat_projection * mat_to_ndc_coordinates;
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

            engine->render(warrior_vertex_buffer,
                           warrior_index_buffer,
                           warrior_sprite->get_sprite(),
                           0,
                           direction,
                           &mat_result[0][0]);
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
