#include "engine.hxx"
#include "buttons.hxx"
#include "shader_program.hxx"

#include <algorithm>
#include <array>
#include <stdexcept>

#include <SDL3/SDL.h>
#include <SDL3/SDL_syswm.h>

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#include <android/log.h>
#else
#include "glad/glad.h"
#endif

#include <glm/matrix.hpp>

#include "imgui.h"

bool check_pressing_key(SDL_Event sdl_event, event& event);
bool check_pressing_button(SDL_Event             sdl_event,
                           event&                event,
                           std::vector<buttons>& buttons_,
                           float                 window_width,
                           float                 window_height);
bool is_key_down(key key_);

void gl_check();

bool mouse_button_pressed = false;

void        ImGui_ImplGameEngine_Init(SDL_Window* window);
bool        ImGui_ImplGameEngine_ProcessEvent(const SDL_Event* event);
void        ImGui_ImplGameEngine_NewFrame(SDL_Window* window);
void        ImGui_ImplGameEngine_CreateDeviceObject();
void        ImGui_ImplGameEngine_DestroyDeviceObject();
void        ImGui_ImplGameEngine_RenderDrawData(ImDrawData* draw_data);
void        ImGui_ImplGameEngine_Shutdown();
static void ImGui_ImplGameEngine_UpdateMouseData(SDL_Window* window);
static void ImGui_ImplGameEngine_SetClipboardText(void*, const char* text);
static const char* ImGui_ImplGameEngine_GetClipboardText(void*);

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

class game_engine final : public engine
{
    SDL_Window* window;
    int         window_width         = 0;
    int         window_height        = 0;
    int         window_width_pixels  = 0;
    int         window_height_pixels = 0;

    SDL_GLContext   context;
    shader_program* hero_program;
    shader_program* map_program;

    SDL_AudioDeviceID          audio_device;
    SDL_AudioSpec              device_audio_spec;
    std::vector<sound_buffer*> sounds;

    std::vector<buttons> mobile_buttons;
    index_buffer*        buttons_index_buffer;

    std::basic_streambuf<char>* cout_buf = nullptr;
    std::basic_streambuf<char>* cerr_buf = nullptr;

    static void audio_callback(void* ptr, uint8_t* stream, int stream_size)
    {
        std::fill_n(stream, stream_size, '\0');

        game_engine* engine = static_cast<game_engine*>(ptr);

        for (sound_buffer* snd : engine->sounds)
        {
            snd->lock_thread();
            if (snd->get_playing_status())
            {
                uint32_t rest = snd->get_length() - snd->get_current_index();
                uint8_t* current_buff =
                    &snd->get_buffer()[snd->get_current_index()];

                if (rest <= static_cast<uint32_t>(stream_size))
                {
                    SDL_MixAudioFormat(stream,
                                       current_buff,
                                       engine->device_audio_spec.format,
                                       rest,
                                       SDL_MIX_MAXVOLUME);
                    snd->update_buffer(rest);
                }
                else
                {
                    SDL_MixAudioFormat(stream,
                                       current_buff,
                                       engine->device_audio_spec.format,
                                       static_cast<uint32_t>(stream_size),
                                       SDL_MIX_MAXVOLUME);
                    snd->update_buffer(static_cast<uint32_t>(stream_size));
                }

                if (snd->get_current_index() == snd->get_length())
                {
                    if (snd->get_loop_property())
                        snd->replay();
                    else
                        snd->stop();
                }
            }
            snd->unlock_thread();
        }
    }

public:
    game_engine()
        : window(nullptr)
        , context(nullptr)
        , hero_program(nullptr)
        , map_program(nullptr)
    {
    }
    bool initialize() final
    {
        android_redirected_buf logcat;

        cout_buf = std::cout.rdbuf();
        cerr_buf = std::cerr.rdbuf();

#ifdef __ANDROID__
        std::cout.rdbuf(&logcat);
        std::cerr.rdbuf(&logcat);
#endif

        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
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
        auto       os_list = { "Windows"sv, "Mac OS X"sv };
        const auto it = std::find(os_list.begin(), os_list.end(), platform);
        int        gl_major_version;
        int        gl_minor_version;
        int        gl_profile_mask;
        if (it != os_list.end())
        {
            gl_major_version = (platform == "Windows"sv) ? 2 : 4;
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

        SDL_GL_SetSwapInterval(1);

        std::cout << "OpenGL " << gl_major_version << '.' << gl_minor_version
                  << std::endl;
        SDL_ShowWindow(window);

#ifndef __ANDROID__
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

        glEnable(GL_BLEND);
        gl_check();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        gl_check();
        glDisable(GL_CULL_FACE);
        gl_check();
        glDisable(GL_DEPTH_TEST);
        gl_check();
        glDisable(GL_STENCIL_TEST);
        gl_check();
        glEnable(GL_SCISSOR_TEST);
        gl_check();

        SDL_GetWindowSizeInPixels(
            window, &window_width_pixels, &window_height_pixels);
        std::cout << window_width_pixels << " " << window_height_pixels
                  << std::endl;
        glViewport(0, 0, window_width_pixels, window_height_pixels);

        ImGui_ImplGameEngine_Init(window);

        SDL_AudioSpec desired_audio_spec{};
        desired_audio_spec.freq     = 44100;
        desired_audio_spec.format   = AUDIO_S16LSB;
        desired_audio_spec.channels = 2;
        desired_audio_spec.samples  = 1024;
        desired_audio_spec.callback = audio_callback;
        desired_audio_spec.userdata = this;

        const char* audio_device_name = nullptr;
        const int   num_audio_devices = SDL_GetNumAudioDevices(SDL_FALSE);
        for (int i = 0; i < num_audio_devices; i++)
        {
            std::cout << "audio device #" << i << ": "
                      << SDL_GetAudioDeviceName(i, SDL_FALSE) << '\n';
        }
        audio_device_name = SDL_GetAudioDeviceName(0, SDL_FALSE);

        if (platform == "Windows"sv)
            audio_device_name = SDL_GetAudioDeviceName(1, SDL_FALSE);

        audio_device = SDL_OpenAudioDevice(audio_device_name,
                                           SDL_FALSE,
                                           &desired_audio_spec,
                                           &device_audio_spec,
                                           SDL_AUDIO_ALLOW_ANY_CHANGE);

        if (audio_device == 0)
        {
            ImGui_ImplGameEngine_Shutdown();
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        SDL_PlayAudioDevice(audio_device);

        mobile_buttons.resize(2);

        mobile_buttons[0].width  = 270.f;
        mobile_buttons[0].height = 270.f;
        mobile_buttons[0].texture_ =
            create_texture("img/mobile_control_buttons.png");
        mobile_buttons[0].vertices[0] = { 50.f,
                                          window_height_pixels -
                                              mobile_buttons[0].height - 50.f,
                                          0.f,
                                          1.f };
        mobile_buttons[0].vertices[1] = { 50.f + mobile_buttons[0].width,
                                          window_height_pixels -
                                              mobile_buttons[0].height - 50.f,
                                          1.f,
                                          1.f };
        mobile_buttons[0].vertices[2] = { 50.f + mobile_buttons[0].width,
                                          window_height_pixels - 50.f,
                                          1.f,
                                          0.f };
        mobile_buttons[0].vertices[3] = {
            50.f, window_height_pixels - 50.f, 0.f, 0.f
        };
        mobile_buttons[0].vertex_buffer_ = create_vertex_buffer();
        mobile_buttons[0].vertex_buffer_->buffer_data(
            mobile_buttons[0].vertices, static_cast<size_t>(4));

        mobile_buttons[1].width  = 180.f;
        mobile_buttons[1].height = 180.f;
        mobile_buttons[1].texture_ =
            create_texture("img/mobile_attack_button.png");
        mobile_buttons[1].vertices[0] = {
            window_width_pixels - mobile_buttons[1].width - 50.f,
            window_height_pixels - mobile_buttons[1].height - 100.f,
            0.f,
            1.f
        };
        mobile_buttons[1].vertices[1] = { window_width_pixels - 50.f,
                                          window_height_pixels -
                                              mobile_buttons[1].height - 100.f,
                                          1.f,
                                          1.f };
        mobile_buttons[1].vertices[2] = {
            window_width_pixels - 50.f, window_height_pixels - 100.f, 1.f, 0.f
        };
        mobile_buttons[1].vertices[3]    = { window_width_pixels -
                                                 mobile_buttons[1].width - 50.f,
                                             window_height_pixels - 100.f,
                                             0.f,
                                             0.f };
        mobile_buttons[1].vertex_buffer_ = create_vertex_buffer();
        mobile_buttons[1].vertex_buffer_->buffer_data(
            mobile_buttons[1].vertices, static_cast<size_t>(4));

        buttons_index_buffer = create_index_buffer();
        buttons_index_buffer->add_indexes(static_cast<size_t>(4));

        return true;
    }
    bool read_input(event& event) final
    {
        SDL_Event sdl_event;

        bool game_event = false;

        if (SDL_PollEvent(&sdl_event))
        {
            ImGui_ImplGameEngine_ProcessEvent(&sdl_event);
            switch (sdl_event.type)
            {
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP:
                    if (check_pressing_key(sdl_event, event))
                        game_event = true;
                    break;
                case SDL_EVENT_FINGER_DOWN:
                case SDL_EVENT_FINGER_UP:
                    SDL_Event e1;
                    SDL_Event e2;
                    e1.type = SDL_EVENT_FINGER_DOWN;
                    e2.type = SDL_EVENT_FINGER_UP;
                    if (check_pressing_button(sdl_event,
                                              event,
                                              mobile_buttons,
                                              window_width_pixels,
                                              window_height_pixels))
                        game_event = true;
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    mouse_button_pressed = true;
                    break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    mouse_button_pressed = false;
                    break;
                case SDL_EVENT_QUIT:
                    event      = event::turn_off;
                    game_event = true;
                    break;
                default:
                    break;
            }
        }
        return game_event;
    }
    bool          check_key(key key_) final { return is_key_down(key_); }
    sound_buffer* create_sound_buffer(const char* file_path) final
    {
        sound_buffer* sound_buffer = ::create_sound_buffer(
            file_path, reinterpret_cast<void*>(&device_audio_spec));
        sound_buffer->lock_thread();
        sounds.push_back(sound_buffer);
        sound_buffer->unlock_thread();
        return sound_buffer;
    }
    void render(vertex_buffer* vertex_buffer,
                index_buffer*  index_buffer,
                sprite*        sprite,
                int            direction,
                float*         matrix_first_value) final
    {
        hero_program->use();
        hero_program->set_uniform_1f(
            "quantity", static_cast<float>(sprite->get_quantity()));
        hero_program->set_uniform_1f(
            "number",
            static_cast<float>(sprite->get_current_number(direction)));
        hero_program->set_uniform_1f("start_position",
                                     sprite->get_start_position());
        hero_program->set_uniform_1f(
            "width",
            sprite->get_width() /
                static_cast<float>(sprite->get_texture()->get_width()));
        hero_program->set_uniform_1i("direction", direction);
        hero_program->set_uniform_1i("texture", 0);
        hero_program->set_uniform_matrix4fv(
            "matrix", 1, GL_FALSE, matrix_first_value);

        sprite->get_texture()->active(0);
        sprite->get_texture()->bind();

        vertex_buffer->bind();
        index_buffer->bind();

        glVertexAttribPointer(0,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_2d),
                              reinterpret_cast<void*>(0));
        gl_check();
        glEnableVertexAttribArray(0);
        gl_check();

        glVertexAttribPointer(1,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_2d),
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
                texture*       texture,
                float*         matrix_first_value) final
    {
        map_program->use();
        map_program->set_uniform_1i("texture", 0);
        map_program->set_uniform_matrix4fv(
            "matrix", 1, GL_FALSE, matrix_first_value);

        texture->active(0);
        texture->bind();

        vertex_buffer->bind();
        index_buffer->bind();

        glVertexAttribPointer(0,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_2d),
                              reinterpret_cast<void*>(0));
        gl_check();
        glEnableVertexAttribArray(0);
        gl_check();

        glVertexAttribPointer(1,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex_2d),
                              reinterpret_cast<void*>(2 * sizeof(float)));
        gl_check();
        glEnableVertexAttribArray(1);
        gl_check();

        glDrawElements(
            GL_TRIANGLES, index_buffer->get_size(), GL_UNSIGNED_SHORT, 0);
        gl_check();
    }
    void render_buttons(float* matrix_first_value) final
    {
        for (auto button : mobile_buttons)
            render(button.vertex_buffer_,
                   buttons_index_buffer,
                   button.texture_,
                   matrix_first_value);
    }
    bool render_gui(bool& show_menu_window, gui_type type) final
    {
        ImGui_ImplGameEngine_NewFrame(window);

        int menu_width  = window_width / 4;
        int menu_height = window_height / 4;

        ImGui::SetNextWindowPos(ImVec2((window_width - menu_width) / 2,
                                       (window_height - menu_height) / 2),
                                ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(menu_width, menu_height),
                                 ImGuiCond_Always);

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
        if (ImGui::Button("Exit",
                          ImVec2(menu_width - 20, menu_height / 2 - 15)))
            close_window = true;

        ImGui::End();

        ImGui::Render();
        ImGui_ImplGameEngine_RenderDrawData(ImGui::GetDrawData());

        return close_window;
    }
    void clear() final
    {
        glClearColor(0.1f, 0.1f, 0.1f, 0.f);
        gl_check();
        glClear(GL_COLOR_BUFFER_BIT);
        gl_check();
    }
    float get_time() final
    {
        std::uint32_t ms = SDL_GetTicks();
        return ms * 0.001f;
    }
    int  get_window_width() final { return window_width; }
    int  get_window_height() final { return window_height; }
    int  get_window_width_pixels() final { return window_width_pixels; }
    int  get_window_height_pixels() final { return window_height_pixels; }
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
        std::cout.rdbuf(cout_buf);
        std::cerr.rdbuf(cerr_buf);

        ImGui_ImplGameEngine_Shutdown();
        delete hero_program;

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

// Dear ImGui setup

shader_program* imgui_shader_program = nullptr;
uint64_t        imgui_time           = 0;
GLuint          imgui_VBO            = 0;
GLuint          imgui_EBO            = 0;

void ImGui_ImplGameEngine_Init(SDL_Window* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    io.BackendPlatformName = "custom_game_engine";

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

    io.SetClipboardTextFn = ImGui_ImplGameEngine_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplGameEngine_GetClipboardText;
    io.ClipboardUserData  = nullptr;

    ImGuiViewport* main_viewport     = ImGui::GetMainViewport();
    main_viewport->PlatformHandleRaw = nullptr;

    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
    SDL_SetHint(SDL_HINT_MOUSE_AUTO_CAPTURE, "0");

#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_GetWindowWMInfo(window, &wmInfo, SDL_SYSWM_CURRENT_VERSION);
    io.ImeWindowHandle = wmInfo.info.win.window;
#endif

    imgui_time = SDL_GetPerformanceCounter();
}

bool ImGui_ImplGameEngine_ProcessEvent(const SDL_Event* event)
{
    ImGuiIO& io = ImGui::GetIO();

    switch (event->type)
    {
        case SDL_EVENT_MOUSE_MOTION:
        {
            ImVec2 mouse_pos((float)event->motion.x, (float)event->motion.y);
            io.AddMouseSourceEvent(event->motion.which == SDL_TOUCH_MOUSEID
                                       ? ImGuiMouseSource_TouchScreen
                                       : ImGuiMouseSource_Mouse);
            io.AddMousePosEvent(mouse_pos.x, mouse_pos.y);
            return true;
        }
        case SDL_EVENT_MOUSE_WHEEL:
        {
            float wheel_x = -event->wheel.x;
            float wheel_y = event->wheel.y;
            io.AddMouseSourceEvent(event->wheel.which == SDL_TOUCH_MOUSEID
                                       ? ImGuiMouseSource_TouchScreen
                                       : ImGuiMouseSource_Mouse);
            io.AddMouseWheelEvent(wheel_x, wheel_y);
            return true;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            int mouse_button = -1;
            if (event->button.button == SDL_BUTTON_LEFT)
                mouse_button = 0;
            if (event->button.button == SDL_BUTTON_RIGHT)
                mouse_button = 1;
            if (event->button.button == SDL_BUTTON_MIDDLE)
                mouse_button = 2;
            if (event->button.button == SDL_BUTTON_X1)
                mouse_button = 3;
            if (event->button.button == SDL_BUTTON_X2)
                mouse_button = 4;
            if (mouse_button == -1)
                break;
            io.AddMouseSourceEvent(event->button.which == SDL_TOUCH_MOUSEID
                                       ? ImGuiMouseSource_TouchScreen
                                       : ImGuiMouseSource_Mouse);
            io.AddMouseButtonEvent(
                mouse_button, (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN));
            return true;
        }
    }
    return false;
}

void ImGui_ImplGameEngine_NewFrame(SDL_Window* window)
{
    if (imgui_shader_program == nullptr)
        ImGui_ImplGameEngine_CreateDeviceObject();

    ImGuiIO& io = ImGui::GetIO();

    int width, height;
    int display_width, display_height;
    SDL_GetWindowSize(window, &width, &height);
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        width = height = 0;
    SDL_GetWindowSizeInPixels(window, &display_width, &display_height);

    io.DisplaySize =
        ImVec2(static_cast<float>(width), static_cast<float>(height));
    if (width > 0 && height > 0)
        io.DisplayFramebufferScale =
            ImVec2(static_cast<float>(display_width) / width,
                   static_cast<float>(display_height) / height);

    static uint64_t frequency    = SDL_GetPerformanceFrequency();
    uint64_t        current_time = SDL_GetPerformanceCounter();
    if (current_time < imgui_time)
        current_time = imgui_time + 1;

    io.DeltaTime =
        imgui_time > 0
            ? (float)((double)(current_time - imgui_time) / frequency)
            : (float)(1.0f / 60.0f);
    imgui_time = current_time;

    ImGui_ImplGameEngine_UpdateMouseData(window);

    ImGui::NewFrame();
}

void ImGui_ImplGameEngine_CreateDeviceObject()
{
    imgui_shader_program = create_shader_program();

    if (!imgui_shader_program->create_shader("shaders/vertex_shader_imgui.glsl",
                                             shader_type::vertex))
        return;

    imgui_shader_program->bind("Position", 0);
    imgui_shader_program->bind("UV", 1);
    imgui_shader_program->bind("Color", 2);

    if (!imgui_shader_program->create_shader(
            "shaders/fragment_shader_imgui.glsl", shader_type::fragment))
        return;

    if (!imgui_shader_program->link())
        return;

    glGenBuffers(1, &imgui_VBO);
    gl_check();
    glGenBuffers(1, &imgui_EBO);
    gl_check();

    ImGuiIO& io = ImGui::GetIO();

    int            width, height;
    unsigned char* pixels = nullptr;

    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines;
    texture* texture = create_texture(pixels, width, height);
    io.Fonts->SetTexID((void*)texture);
}

void ImGui_ImplGameEngine_DestroyDeviceObject()
{
    glDeleteBuffers(1, &imgui_VBO);
    gl_check();
    glDeleteBuffers(1, &imgui_EBO);
    gl_check();
    delete imgui_shader_program;
}

void ImGui_ImplGameEngine_RenderDrawData(ImDrawData* draw_data)
{
    ImGuiIO& io = ImGui::GetIO();

    int fb_width =
        static_cast<int>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height =
        static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);

    GLint last_scissor_box[4];
    glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);

    glActiveTexture(GL_TEXTURE0);

    glViewport(0, 0, fb_width, fb_height);
    gl_check();

    float L = draw_data->DisplayPos.x;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

    glm::mat4 ortho_projection{
        { 2.0f / (R - L), 0.0f, 0.0f, 0.0f },
        { 0.0f, 2.0f / (T - B), 0.0f, 0.0f },
        { 0.0f, 0.0f, -1.0f, 0.0f },
        { (R + L) / (L - R), (T + B) / (B - T), 0.0f, 1.0f },
    };

    imgui_shader_program->use();
    imgui_shader_program->set_uniform_1i("Texture", 0);
    imgui_shader_program->set_uniform_matrix4fv(
        "ProjMtx", 1, GL_FALSE, &ortho_projection[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, imgui_VBO);
    gl_check();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imgui_EBO);
    gl_check();

    glEnableVertexAttribArray(0);
    gl_check();
    glVertexAttribPointer(0,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(ImDrawVert),
                          (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
    gl_check();

    glEnableVertexAttribArray(1);
    gl_check();
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(ImDrawVert),
                          (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));

    glEnableVertexAttribArray(2);
    gl_check();
    glVertexAttribPointer(2,
                          4,
                          GL_UNSIGNED_BYTE,
                          GL_TRUE,
                          sizeof(ImDrawVert),
                          (GLvoid*)IM_OFFSETOF(ImDrawVert, col));
    gl_check();

    ImVec2 clip_off   = draw_data->DisplayPos;
    ImVec2 clip_scale = draw_data->FramebufferScale;

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const GLsizeiptr  vtx_buffer_size =
            (GLsizeiptr)cmd_list->VtxBuffer.Size * (int)sizeof(ImDrawVert);
        const GLsizeiptr idx_buffer_size =
            (GLsizeiptr)cmd_list->IdxBuffer.Size * (int)sizeof(ImDrawIdx);
        glBufferData(GL_ARRAY_BUFFER,
                     vtx_buffer_size,
                     (const GLvoid*)cmd_list->VtxBuffer.Data,
                     GL_STREAM_DRAW);
        gl_check();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     idx_buffer_size,
                     (const GLvoid*)cmd_list->IdxBuffer.Data,
                     GL_STREAM_DRAW);
        gl_check();

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

            // Project scissor/clipping rectangles into framebuffer space
            ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x,
                            (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
            ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x,
                            (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
            if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                continue;

            // Apply scissor/clipping rectangle (Y is inverted in OpenGL)
            glScissor((int)clip_min.x,
                      (int)((float)fb_height - clip_max.y),
                      (int)(clip_max.x - clip_min.x),
                      (int)(clip_max.y - clip_min.y));
            gl_check();
            // Bind texture, Draw
            texture* imgui_texture = (texture*)pcmd->GetTexID();
            imgui_texture->bind();
            glDrawElements(
                GL_TRIANGLES,
                (GLsizei)pcmd->ElemCount,
                sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)));
            gl_check();
        }
    }

    glScissor(last_scissor_box[0],
              last_scissor_box[1],
              (GLsizei)last_scissor_box[2],
              (GLsizei)last_scissor_box[3]);
}

void ImGui_ImplGameEngine_Shutdown()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplGameEngine_DestroyDeviceObject();
    texture* imgui_texture = (texture*)io.Fonts->TexID;
    delete imgui_texture;
    io.Fonts->SetTexID(0);

    io.BackendPlatformName = nullptr;

    ImGui::DestroyContext();
}

void ImGui_ImplGameEngine_UpdateMouseData(SDL_Window* window)
{
    ImGuiIO&    io             = ImGui::GetIO();
    SDL_Window* focused_window = SDL_GetKeyboardFocus();
    const bool  app_is_focused = (window == focused_window);
    if (app_is_focused)
    {
        if (io.WantSetMousePos)
            SDL_WarpMouseInWindow(window, io.MousePos.x, io.MousePos.y);

        if (mouse_button_pressed)
        {
            float mouse_x_global, mouse_y_global;
            int   window_x, window_y;
            SDL_GetGlobalMouseState(&mouse_x_global, &mouse_y_global);
            SDL_GetWindowPosition(focused_window, &window_x, &window_y);
            io.AddMousePosEvent(mouse_x_global - window_x,
                                mouse_y_global - window_y);
        }
    }
}

void ImGui_ImplGameEngine_SetClipboardText(void*, const char* text)
{
    SDL_SetClipboardText(text);
}

const char* ImGui_ImplGameEngine_GetClipboardText(void*)
{
    return SDL_GetClipboardText();
}
