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

    int direction = 0;

    bool  quit = false;
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

        // engine->render(texture_triangles[0]);
        // engine->render(texture_triangles[1]);

        if (!engine->swap_buffers())
        {
            engine->uninitialize();
            return EXIT_FAILURE;
        }
    }

    delete knight_texture;
    engine->uninitialize();

    return EXIT_SUCCESS;
}
