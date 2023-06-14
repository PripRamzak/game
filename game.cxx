#include "engine.hxx"

#include <chrono>
#include <cstdlib>
#include <fstream>

int main(int /*argc*/, char** /*argv*/)
{
    std::unique_ptr<engine, void (*)(engine*)> engine(create_engine(),
                                                      destroy_engine);

    if (!engine->initialize())
        return EXIT_FAILURE;

    std::vector<triangle_2d> texture_triangles;
    texture_triangles.resize(2);

    texture* warrior_idle_sprite_sheet = create_texture();
    warrior_idle_sprite_sheet->load("./img/warrior_idle.png", 6, 0);
    engine->create_triangles(warrior_idle_sprite_sheet, texture_triangles);

    vertex_buffer* warrior_vertex_buffer = create_vertex_buffer();
    warrior_vertex_buffer->buffer_data(texture_triangles,
                                       texture_triangles.size());

    std::vector<uint16_t> indexes              = { 0, 1, 2, 3, 4, 5 };
    index_buffer*         warrior_index_buffer = create_index_buffer();
    warrior_index_buffer->buffer_data(indexes, indexes.size());

    int direction = 0;

    float last_time        = engine->get_time();
    bool  quit             = false;
    bool  show_menu_window = true;
    event event;

    while (!quit)
    {

        if (engine->read_input(event))
        {
            std::cout << event << std::endl;
            if (event == event::up_pressed)
            {
                engine->move_texture(texture_triangles, 0.f, 0.03f, direction);
                warrior_vertex_buffer->buffer_data(texture_triangles,
                                                   texture_triangles.size());
                warrior_idle_sprite_sheet->next_texture();
            }
            else if (event == event::down_pressed)
            {
                engine->move_texture(texture_triangles, 0.f, -0.03f, direction);
                warrior_vertex_buffer->buffer_data(texture_triangles,
                                                   texture_triangles.size());
                warrior_idle_sprite_sheet->next_texture();
            }
            else if (event == event::left_pressed)
            {
                engine->move_texture(texture_triangles, -0.03f, 0.f, direction);
                warrior_vertex_buffer->buffer_data(texture_triangles,
                                                   texture_triangles.size());
                warrior_idle_sprite_sheet->next_texture();
            }
            else if (event == event::right_pressed)
            {
                engine->move_texture(texture_triangles, 0.03f, 0.f, direction);
                warrior_vertex_buffer->buffer_data(texture_triangles,
                                                   texture_triangles.size());
                warrior_idle_sprite_sheet->next_texture();
            }
            if (event == event::turn_off)
            {
                quit = true;
                break;
            }
        }

        if (show_menu_window)
            engine->render_menu(show_menu_window);
        else
        {
            engine->render(warrior_vertex_buffer,
                           warrior_index_buffer,
                           warrior_idle_sprite_sheet,
                           0);
            if (engine->get_time() - last_time > 0.15f)
            {
                warrior_idle_sprite_sheet->next_texture();
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
