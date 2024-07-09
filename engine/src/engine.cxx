#include "include/engine.hxx"
#include "include/buttons.hxx"
#include "include/shader_program.hxx"

#include <algorithm>
#include <array>
#include <chrono>
#include <stdexcept>

#include <SDL3/SDL.h>
// #include <SDL3/SDL_syswm.h>

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#include <android/log.h>
#else
#include "glad/glad.h"
#endif

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

namespace prip_engine
{
bool check_pressing_key(SDL_Event sdl_event, event& event);
/*bool check_pressing_button(SDL_Event             sdl_event,
                           event&                event,
                           std::vector<buttons>& buttons_,
                           float                 window_width,
                           float                 window_height);*/
void gl_check();

class android_redirected_buf : public std::streambuf
{
    std::string message;

    int overflow(int c) override
    {
        if (c == EOF)
            return !EOF;
        else
        {
            if (c == '\n')
            {
#ifdef __ANDROID__
                __android_log_print(
                    ANDROID_LOG_ERROR, "prip", "%s", message.c_str());
#endif
                message.clear();
            }
            else
                message.push_back(static_cast<char>(c));
        }
        return c;
    }
    int sync() override { return 0; }

public:
    android_redirected_buf() = default;
};

static bool engine_init = false;

static SDL_Window* window;
static int         window_width         = 0;
static int         window_height        = 0;
static int         window_width_pixels  = 0;
static int         window_height_pixels = 0;

static SDL_GLContext   context;
static shader_program* hero_program;
static shader_program* map_program;
static shader_program* sprite_program;
static shader_program* collider_program;

static index_buffer* solo_objects_index_buffer;

static std::vector<buttons> mobile_buttons;
static index_buffer*        buttons_index_buffer;

static std::chrono::high_resolution_clock timer;

static std::basic_streambuf<char>* cout_buf = nullptr;
static std::basic_streambuf<char>* cerr_buf = nullptr;

bool init()
{
    if (engine_init)
        throw std::runtime_error("Engine already exists");

    android_redirected_buf logcat;

    cout_buf = std::cout.rdbuf();
    cerr_buf = std::cerr.rdbuf();

#ifdef __ANDROID__
    std::cout.rdbuf(&logcat);
    std::cerr.rdbuf(&logcat);
#endif

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Init error : " << SDL_GetError() << std::endl;
        return false;
    }

    window_width  = 1280;
    window_height = 720;

#ifdef __ANDROID__
    const SDL_DisplayMode* display_mode = SDL_GetCurrentDisplayMode(1);
    if (!display_mode)
    {
        std::cerr << "Can't get current display mode: " << SDL_GetError()
                  << std::endl;
        SDL_Quit();
        return false;
    }
    window_width  = display_mode->screen_w;
    window_height = display_mode->screen_h;

    std::cout << window_width << " " << window_height << std::endl;
#endif

    window = SDL_CreateWindow(
        "Game", window_width, window_height, SDL_WINDOW_OPENGL);
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
    int gl_major_version;
    int gl_minor_version;
    int gl_profile_mask;
    if (platform == "Windows"sv)
    {
        gl_major_version = 2;
        gl_minor_version = 1;
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

    if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_version))
    {
        std::cerr << "GL_GetAttribute error: " << SDL_GetError() << std::endl;
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
    else if (platform == "Windows"sv && gl_major_version < 2 &&
             gl_minor_version < 1)
    {
        std::cerr << "Current OpenGL version: " << gl_major_version << '.'
                  << gl_minor_version << std::endl;
        std::cerr << "At least need OpenGL version: 4.3" << std::endl;
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    std::cout << "OpenGL " << gl_major_version << '.' << gl_minor_version
              << std::endl;
    SDL_ShowWindow(window);

#ifndef __ANDROID__
    // load OpenGL functions
    auto gl_function_loader = [](const char* function_name)
    {
        SDL_FunctionPointer function_ptr = SDL_GL_GetProcAddress(function_name);
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
#endif

    hero_program = create_shader_program();
    if (!hero_program->create_shader("shaders/vertex_hero_shader.glsl",
                                     shader_type::vertex))
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    hero_program->bind("vertex_position", 0);
    hero_program->bind("texture_coordinates", 1);

    if (!hero_program->create_shader("shaders/fragment_hero_shader.glsl",
                                     shader_type::fragment))
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    if (!hero_program->link())
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    map_program = create_shader_program();

    if (!map_program->create_shader("shaders/vertex_map_shader.glsl",
                                    shader_type::vertex))
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    map_program->bind("vertex_position", 0);
    map_program->bind("texture_coordinates", 1);

    if (!map_program->create_shader("shaders/fragment_map_shader.glsl",
                                    shader_type::fragment))
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    if (!map_program->link())
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    sprite_program = create_shader_program();

    if (!sprite_program->create_shader("shaders/sprite.vert",
                                       shader_type::vertex))
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    if (!sprite_program->create_shader("shaders/sprite.frag",
                                       shader_type::fragment))
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    sprite_program->bind("vertex_position", 0);
    sprite_program->bind("text_coord", 1);

    if (!sprite_program->link())
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    collider_program = create_shader_program();

    if (!collider_program->create_shader("shaders/collider.vert",
                                         shader_type::vertex))
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    collider_program->bind("vertex_position", 0);
    collider_program->bind("vertex_color", 1);

    if (!collider_program->create_shader("shaders/collider.frag",
                                         shader_type::fragment))
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    if (!collider_program->link())
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    glEnable(GL_BLEND);
    gl_check();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl_check();
    glLineWidth(3);
    gl_check();
    glClearColor(24.f / 255.f, 20.f / 255.f, 37.f / 255.f, 0.f);
    gl_check();

    SDL_GetWindowSizeInPixels(
        window, &window_width_pixels, &window_height_pixels);
    std::cout << window_width_pixels << " " << window_height_pixels
              << std::endl;
    glViewport(0, 0, window_width_pixels, window_height_pixels);

    solo_objects_index_buffer = create_index_buffer();
    solo_objects_index_buffer->add_indexes(primitives::triangle, 1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 100");

    if (!init_sound())
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    /*mobile_buttons.resize(2);

    mobile_buttons[0].width  = 270.f;
    mobile_buttons[0].height = 270.f;
    mobile_buttons[0].texture_ =
        create_texture("img/mobile_control_buttons.png");
    mobile_buttons[0].vertex_buffer_ = create_vertex_buffer();
    mobile_buttons[0].vertex_buffer_->add_vertex(
        { 50.f,
          window_height_pixels - mobile_buttons[0].height - 50.f,
          0.f,
          1.f });
    mobile_buttons[0].vertex_buffer_->add_vertex(
        { 50.f + mobile_buttons[0].width,
          window_height_pixels - mobile_buttons[0].height - 50.f,
          1.f,
          1.f });
    mobile_buttons[0].vertex_buffer_->add_vertex(
        { 50.f + mobile_buttons[0].width,
          window_height_pixels - 50.f,
          1.f,
          0.f });
    mobile_buttons[0].vertex_buffer_->add_vertex(
        { 50.f, window_height_pixels - 50.f, 0.f, 0.f });
    mobile_buttons[0].vertex_buffer_->buffer_data();

    mobile_buttons[1].width  = 180.f;
    mobile_buttons[1].height = 180.f;
    mobile_buttons[1].texture_ =
        create_texture("img/mobile_attack_button.png");
    mobile_buttons[1].vertex_buffer_ = create_vertex_buffer();
    mobile_buttons[1].vertex_buffer_->add_vertex(
        { window_width_pixels - mobile_buttons[1].width - 50.f,
          window_height_pixels - mobile_buttons[1].height - 100.f,
          0.f,
          1.f });
    mobile_buttons[1].vertex_buffer_->add_vertex(
        { window_width_pixels - 50.f,
          window_height_pixels - mobile_buttons[1].height - 100.f,
          1.f,
          1.f });
    mobile_buttons[1].vertex_buffer_->add_vertex(
        { window_width_pixels - 50.f,
          window_height_pixels - 100.f,
          1.f,
          0.f });
    mobile_buttons[1].vertex_buffer_->add_vertex(
        { window_width_pixels - mobile_buttons[1].width - 50.f,
          window_height_pixels - 100.f,
          0.f,
          0.f });
    mobile_buttons[1].vertex_buffer_->buffer_data();

    buttons_index_buffer = create_index_buffer();
    buttons_index_buffer->add_indexes(static_cast<size_t>(4));*/

    engine_init = true;

    return true;
}

bool read_input(event& event)
{
    SDL_Event sdl_event;

    if (SDL_PollEvent(&sdl_event))
    {
        ImGui_ImplSDL3_ProcessEvent(&sdl_event);
        switch (sdl_event.type)
        {
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                if (check_pressing_key(sdl_event, event))
                    return true;
                break;
            case SDL_EVENT_FINGER_DOWN:
            case SDL_EVENT_FINGER_UP:
                SDL_Event e1;
                SDL_Event e2;
                e1.type = SDL_EVENT_FINGER_DOWN;
                e2.type = SDL_EVENT_FINGER_UP;
                /*if (check_pressing_button(sdl_event,
                                          event,
                                          mobile_buttons,
                                          window_width_pixels,
                                          window_height_pixels))
                    return true;*/
                break;
            case SDL_EVENT_QUIT:
                event = event::turn_off;
                return true;
            case SDL_EVENT_MOUSE_MOTION:
                return true;
            default:
                break;
        }
    }
    return false;
}

void render(animation* anim_sprite, float* matrix)
{
    sprite*  sprite_  = anim_sprite->get_sprite();
    texture* texture_ = sprite_->get_texture();

    hero_program->use();
    hero_program->set_uniform_1f(
        "quantity", static_cast<float>(anim_sprite->get_frames_quantity()));
    hero_program->set_uniform_1f(
        "number", static_cast<float>(anim_sprite->get_current_frame_number()));
    hero_program->set_uniform_1i("texture", 0);
    hero_program->set_uniform_matrix4fv("matrix", 1, GL_FALSE, matrix);

    texture_->active(0);
    texture_->bind();

    sprite_->get_vertex_buffer()->bind();
    solo_objects_index_buffer->bind();

    glVertexAttribPointer(0,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vertex2d_uv),
                          reinterpret_cast<void*>(0));
    gl_check();
    glEnableVertexAttribArray(0);
    gl_check();

    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vertex2d_uv),
                          reinterpret_cast<void*>(2 * sizeof(float)));
    gl_check();
    glEnableVertexAttribArray(1);
    gl_check();

    glDrawElements(GL_TRIANGLES,
                   solo_objects_index_buffer->get_size(),
                   GL_UNSIGNED_SHORT,
                   0);
    gl_check();
}

void render(sprite* sprite, float* matrix)
{
    sprite_program->use();
    sprite_program->set_uniform_1i("texture", 0);
    sprite_program->set_uniform_matrix4fv("mvp", 1, GL_FALSE, matrix);

    sprite->get_texture()->active(0);
    sprite->get_texture()->bind();

    sprite->get_vertex_buffer()->bind();
    solo_objects_index_buffer->bind();

    glVertexAttribPointer(0,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vertex2d_uv),
                          reinterpret_cast<void*>(0));
    gl_check();
    glEnableVertexAttribArray(0);
    gl_check();

    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vertex2d_uv),
                          reinterpret_cast<void*>(2 * sizeof(float)));
    gl_check();
    glEnableVertexAttribArray(1);
    gl_check();

    glDrawElements(GL_TRIANGLES,
                   solo_objects_index_buffer->get_size(),
                   GL_UNSIGNED_SHORT,
                   0);
    gl_check();
}

void render(texture*       texture,
            vertex_buffer* vertex_buffer,
            index_buffer*  index_buffer,
            transform2d    min_uv,
            transform2d    max_uv,
            float*         matrix)
{
    map_program->use();
    map_program->set_uniform_1i("texture", 0);
    map_program->set_uniform_2fv("min_uv", 1, min_uv);
    map_program->set_uniform_2fv("max_uv", 1, max_uv);
    map_program->set_uniform_matrix4fv("matrix", 1, GL_FALSE, matrix);

    texture->active(0);
    texture->bind();

    vertex_buffer->bind();
    index_buffer->bind();

    glVertexAttribPointer(0,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vertex2d_uv),
                          reinterpret_cast<void*>(0));
    gl_check();
    glEnableVertexAttribArray(0);
    gl_check();

    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vertex2d_uv),
                          reinterpret_cast<void*>(2 * sizeof(float)));
    gl_check();
    glEnableVertexAttribArray(1);
    gl_check();

    glDrawElements(
        GL_TRIANGLES, index_buffer->get_size(), GL_UNSIGNED_SHORT, 0);
    gl_check();
}

void render(vertex_buffer* vertex_buffer,
            index_buffer*  index_buffer,
            float*         matrix)
{
    collider_program->use();
    collider_program->set_uniform_matrix4fv("matrix", 1, GL_FALSE, matrix);

    vertex_buffer->bind();
    index_buffer->bind();

    glVertexAttribPointer(0,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vertex2d_color),
                          reinterpret_cast<void*>(0));
    gl_check();
    glEnableVertexAttribArray(0);
    gl_check();

    glVertexAttribPointer(1,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vertex2d_color),
                          reinterpret_cast<void*>(2 * sizeof(float)));
    gl_check();
    glEnableVertexAttribArray(1);
    gl_check();

    glDrawElements(GL_LINES, index_buffer->get_size(), GL_UNSIGNED_SHORT, 0);
    gl_check();
}

/*void render_buttons(float* matrix) final
{
    for (auto button : mobile_buttons)
        render(button.texture_,
               button.vertex_buffer_,
               buttons_index_buffer,
               matrix);
}*/

bool render_gui(bool& show_menu_window, gui_type type)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    int menu_width  = window_width / 4;
    int menu_height = window_height / 4;

    ImGui::SetNextWindowPos(ImVec2((window_width - menu_width) / 2,
                                   (window_height - menu_height) / 2),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(menu_width, menu_height), ImGuiCond_Always);

    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_NoTitleBar;

    ImGui::Begin("Menu", nullptr, flags);

    ImGui::SetCursorPos(ImVec2(10, 10));
    if (type == gui_type::menu)
    {
        if (ImGui::Button("Play",
                          ImVec2(menu_width - 20, menu_height / 2 - 15)))
            show_menu_window = false;
    }
    else
    {
        if (ImGui::Button("Continue",
                          ImVec2(menu_width - 20, menu_height / 2 - 15)))
            show_menu_window = false;
    }

    ImGui::SetCursorPos(ImVec2(10, menu_height / 2 + 5));
    bool close_window = false;
    if (ImGui::Button("Exit", ImVec2(menu_width - 20, menu_height / 2 - 15)))
        close_window = true;

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return close_window;
}

void clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
    gl_check();
}

timepoint get_time()
{
    return timer.now();
}

int get_window_width()
{
    return window_width;
}

int get_window_height()
{
    return window_height;
}

int get_window_width_pixels()
{
    return window_width_pixels;
}

int get_window_height_pixels()
{
    return window_height_pixels;
}

bool swap_buffers()
{
    if (SDL_GL_SwapWindow(window) != 0)
    {
        std::cerr << "Swap Window error:" << SDL_GetError() << std::endl;
        return false;
    }
    clear();
    return true;
}

void destroy()
{
    std::cout.rdbuf(cout_buf);
    std::cerr.rdbuf(cerr_buf);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    delete hero_program;
    delete map_program;
    delete collider_program;
    delete sprite_program;

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
} // namespace prip_engine
