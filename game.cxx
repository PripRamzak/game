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

    texture* knight_texture = create_texture();
    knight_texture->load("./img/knight.png", 0);
    engine->create_triangles(knight_texture, texture_triangles);

    vertex_buffer* knight_vertex_buffer = create_vertex_buffer();
    knight_vertex_buffer->buffer_data(texture_triangles,
                                      texture_triangles.size());

    std::vector<uint16_t> indexes             = { 0, 1, 2, 3, 4, 5 };
    index_buffer*         knight_index_buffer = create_index_buffer();
    knight_index_buffer->buffer_data(indexes, indexes.size());

    int direction = 0;

    bool  quit             = false;
    bool  show_menu_window = true;
    event event;

    while (!quit)
    {

        if (engine->read_input(event))
        {
            std::cout << event << std::endl;
            if (event == event::up_pressed)
                engine->move_texture(texture_triangles, 0.f, 0.03f, direction);
            else if (event == event::down_pressed)
                engine->move_texture(texture_triangles, 0.f, -0.03f, direction);
            else if (event == event::left_pressed)
                engine->move_texture(texture_triangles, -0.03f, 0.f, direction);
            else if (event == event::right_pressed)
                engine->move_texture(texture_triangles, 0.03f, 0.f, direction);
            if (event == event::turn_off)
            {
                quit = true;
                break;
            }
        }

        if (show_menu_window)
            engine->render_menu(show_menu_window);
        else
            engine->render(
                knight_vertex_buffer, knight_index_buffer, knight_texture);

        if (!engine->swap_buffers())
        {
            engine->uninitialize();
            return EXIT_FAILURE;
        }
    }

    knight_texture->delete_texture();
    delete knight_texture;
    engine->uninitialize();

    return EXIT_SUCCESS;
}
