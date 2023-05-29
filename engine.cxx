#include "engine.hxx"
#include "shader_program.hxx"

#include <algorithm>
#include <array>
#include <cassert>
#include <stdexcept>

#include <SDL3/SDL.h>

#include "glad/glad.h"

#include <glm/matrix.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui.h"

bool check_pressing_key(SDL_Event sdl_event, event& event);

void gl_check();

void ImGui_Impl_game_engine_Init();

class game_engine final : public engine
{
    SDL_Window*    window;
    SDL_GLContext  context;
    shader_program program;

public:
    game_engine()
        : window(nullptr)
        , context(nullptr)
    {
    }

    bool initialize() final
    {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
            std::cerr << "Init error : " << SDL_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow("Game", 800, 600, SDL_WINDOW_OPENGL);
        if (!window)
        {
            std::cerr << "CreateWindow error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

        // create context based on os

        std::string_view platform{ SDL_GetPlatform() };
        using namespace std::string_view_literals;
        auto       os_list = { "Windows"sv, "Mac OS X"sv };
        const auto it = std::find(os_list.begin(), os_list.end(), platform);
        int        gl_major_version;
        int        gl_minor_version;
        int        gl_profile_mask;
        if (it != os_list.end())
        {
            gl_major_version = 4;
            gl_minor_version = (platform == "Mac OS X"sv) ? 1 : 3;
            gl_profile_mask  = SDL_GL_CONTEXT_PROFILE_CORE;
        }
        else
        {
            gl_major_version = 2;
            gl_minor_version = 0;
            gl_profile_mask  = SDL_GL_CONTEXT_PROFILE_ES;
        }
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_version);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_version);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_profile_mask);

        context = SDL_GL_CreateContext(window);
        if (context == nullptr)
        {
            std::cerr << "CreateContext error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                                &gl_major_version))
        {
            std::cerr << "GL_GetAttribute error: " << SDL_GetError()
                      << std::endl;
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        if (platform == "Linux"sv &&
            (gl_major_version != 2 || gl_minor_version != 0 ||
             gl_profile_mask != SDL_GL_CONTEXT_PROFILE_ES))
        {
            std::cerr << "Current OpenGL version: " << gl_major_version << '.'
                      << gl_minor_version << " ES" << std::endl;
            std::cerr << "Need OpenGL version: 2.0 ES" << std::endl;
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        else if (platform == "Windows"sv && gl_major_version < 4 &&
                 gl_minor_version < 3)
        {
            std::cerr << "Current OpenGL version: " << gl_major_version << '.'
                      << gl_minor_version << std::endl;
            std::cerr << "At least need OpenGL version: 4.3" << std::endl;
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        else if (platform == "Mac OS X"sv && gl_major_version < 4 &&
                 gl_minor_version < 1)
        {
            std::cerr << "Current OpenGL version: " << gl_major_version << '.'
                      << gl_minor_version << std::endl;
            std::cerr << "At least need OpenGL version: 4.1" << std::endl;
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        std::cout << "OpenGL " << gl_major_version << '.' << gl_minor_version
                  << std::endl;

        // load OpenGL functions
        auto gl_function_loader = [](const char* function_name)
        {
            SDL_FunctionPointer function_ptr =
                SDL_GL_GetProcAddress(function_name);
            return reinterpret_cast<void*>(function_ptr);
        };

        if (!gladLoadGLES2Loader(gl_function_loader))
        {
            std::cerr << "LoadGlFunctions error" << std::endl;
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        if (!program.create_shader("./vertex_shader.glsl", shader_type::vertex))
        {
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        if (!program.create_shader("./fragment_shader.glsl",
                                   shader_type::fragment))
        {
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        if (!program.create())
        {
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        program.bind("a_position", 0);
        program.bind("t_coord", 1);

        return true;
    }

    bool read_input(event& event) final
    {
        SDL_Event sdl_event;

        if (SDL_PollEvent(&sdl_event))
        {
            if (sdl_event.type == SDL_EVENT_QUIT)
            {
                event = event::turn_off;
                return true;
            }
            else if (sdl_event.type == SDL_EVENT_KEY_DOWN ||
                     sdl_event.type == SDL_EVENT_KEY_UP)
                if (check_pressing_key(sdl_event, event))
                    return true;
        }
        return false;
    }

    texture* create_texture(const char*               file_path,
                            int                       index,
                            std::vector<triangle_2d>& texture_triangles) final
    {
        texture* texture = ::create_texture();
        texture->load(file_path, index);
        program.set_uniform(index);

        int window_width;
        int window_height;
        SDL_GetWindowSize(window, &window_width, &window_height);

        float normalized_width =
            static_cast<float>(texture->get_width()) / window_width;
        float normalized_height =
            static_cast<float>(texture->get_height()) / window_height;

        texture_triangles[0] = {
            { -normalized_width, normalized_height, 0.f, 1.f },
            { normalized_width, normalized_height, 1.f, 1.f },
            { normalized_width, -normalized_height, 1.f, 0.f }
        };
        texture_triangles[1] = {
            { -normalized_width, normalized_height, 0.f, 1.f },
            { normalized_width, -normalized_height, 1.f, 0.f },
            { -normalized_width, -normalized_height, 0.f, 0.f }
        };

        return texture;
    }

    void move_texture(std::vector<triangle_2d>& texture_triangles,
                      float                     dx,
                      float                     dy,
                      int&                      direction) final
    {
        bool new_direction = false;
        if (dx < 0 && direction != 1 || dx > 0 && direction != 0)
        {
            new_direction = true;
            direction     = direction == 0 ? 1 : 0;
        }

        glm::mat3 mat_move(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, dx, dy, 1);
        std::vector<triangle_2d> buffer_triangles = texture_triangles;

        for (int i = 0; i < texture_triangles.size(); i++)
            for (int j = 0; j < 3; j++)
            {
                glm::vec3 vec_move(texture_triangles[i].vertices[j].x,
                                   texture_triangles[i].vertices[j].y,
                                   1);

                if (new_direction)
                    buffer_triangles[i].vertices[j].u =
                        buffer_triangles[i].vertices[j].u == 0.f ? 1.f : 0.f;

                vec_move = mat_move * vec_move;

                if (std::abs(vec_move.y) > 1.f || std::abs(vec_move.x) > 1.f)
                    return;

                buffer_triangles[i].vertices[j].x = vec_move.x;
                buffer_triangles[i].vertices[j].y = vec_move.y;
            }

        for (int i = 0; i < texture_triangles.size(); i++)
            texture_triangles[i] = buffer_triangles[i];
    }

    void draw_triangles_2d(const triangle_2d& triangle) final
    {
        glVertexAttribPointer(0,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_2d),
                              &triangle.vertices[0].x);
        gl_check();
        glEnableVertexAttribArray(0);
        gl_check();

        glVertexAttribPointer(1,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_2d),
                              &triangle.vertices[0].u);
        gl_check();
        glEnableVertexAttribArray(1);
        gl_check();

        glValidateProgram(program.get());
        gl_check();
        GLint validate_status = 0;
        glGetProgramiv(program.get(), GL_VALIDATE_STATUS, &validate_status);
        gl_check();
        if (validate_status == GL_FALSE)
        {
            GLint info_length = 0;
            glGetProgramiv(program.get(), GL_INFO_LOG_LENGTH, &info_length);
            gl_check();
            std::vector<char> info_log(static_cast<size_t>(info_length));
            glGetProgramInfoLog(
                program.get(), info_length, nullptr, info_log.data());
            gl_check();

            std::cerr << "Incorrect validate status: " << info_log.data()
                      << std::endl;
            uninitialize();

            throw std::runtime_error("Error");
        }

        glDrawArrays(GL_TRIANGLES, 0, 3);
        gl_check();
    }

    void clear() final
    {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        gl_check();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl_check();
    }

    float get_time() final
    {
        std::uint32_t ms = SDL_GetTicks();
        return ms * 0.001f;
    }

    bool swap_buffers() final
    {
        if (SDL_GL_SwapWindow(window) != 0)
        {
            std::cerr << "Swap Window error:" << SDL_GetError() << std::endl;
            return false;
        }
        clear();

        return true;
    }

    void uninitialize() final
    {
        program.delete_program();
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

static bool engine_exists = false;

engine* create_engine()
{
    if (engine_exists)
        throw std::runtime_error("Engine already exists");
    engine* engine = new game_engine;
    engine_exists  = true;
    return engine;
}

void destroy_engine(engine* engine)
{
    if (!engine_exists)
        throw std::runtime_error("Engine doesn't exists");
    if (engine == nullptr)
        throw std::runtime_error("engine is nullptr");
    delete engine;
}

engine::~engine() = default;

void ImGui_Impl_game_engine_Init()
{
    ImGui::CreateContext();
    ImGuiIO&       io = ImGui::GetIO();
    int            width, height;
    unsigned char* pixels = nullptr;
}