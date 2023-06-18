#include "engine.hxx"
#include "shader_program.hxx"

#include <algorithm>
#include <array>
#include <stdexcept>

#include <SDL3/SDL.h>

#include "glad/glad.h"

#include <glm/matrix.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui.h"

bool check_pressing_key(SDL_Event sdl_event, event& event);

void gl_check();

void        ImGui_ImplGameEngine_Init();
bool        ImGui_ImplGameEngine_ProcessEvent(const SDL_Event* event);
void        ImGui_ImplGameEngine_NewFrame(SDL_Window* window);
void        ImGui_ImplGameEngine_CreateDeviceObject();
void        ImGui_ImplGameEngine_DestroyDeviceObject();
void        ImGui_ImplGameEngine_RenderDrawData(ImDrawData* draw_data);
void        ImGui_ImplGameEngine_Shutdown();
static void ImGui_ImplGameEngine_UpdateMouseData(SDL_Window* window);
static void ImGui_ImplGameEngine_SetClipboardText(void*, const char* text);
static const char* ImGui_ImplGameEngine_GetClipboardText(void*);

class game_engine final : public engine
{
    SDL_Window*     window;
    int             window_width  = 0;
    int             window_height = 0;
    SDL_GLContext   context;
    shader_program* texture_program;

public:
    game_engine()
        : window(nullptr)
        , context(nullptr)
        , texture_program(nullptr)
    {
    }
    bool initialize() final
    {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
            std::cerr << "Init error : " << SDL_GetError() << std::endl;
            return false;
        }

        window_width  = 1280;
        window_height = 720;

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

        SDL_GL_SetSwapInterval(1);

        std::cout << "OpenGL " << gl_major_version << '.' << gl_minor_version
                  << std::endl;
        SDL_ShowWindow(window);

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

        texture_program = create_shader_program();

        if (!texture_program->create_shader("./vertex_shader.glsl",
                                            shader_type::vertex))
        {
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        if (!texture_program->create_shader("./fragment_shader.glsl",
                                            shader_type::fragment))
        {
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        if (!texture_program->link())
        {
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        texture_program->bind("a_position", 0);
        texture_program->bind("t_coord", 1);

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

        ImGui_ImplGameEngine_Init();

        return true;
    }
    bool read_input(event& event) final
    {
        SDL_Event sdl_event;

        if (SDL_PollEvent(&sdl_event))
        {
            ImGui_ImplGameEngine_ProcessEvent(&sdl_event);
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
    void render(vertex_buffer* vertex_buffer,
                index_buffer*  index_buffer,
                texture*       texture,
                int            index,
                int            direction,
                float*         matrix_first_value) final
    {
        texture_program->use();
        texture_program->set_uniform_1f(
            "quantity", static_cast<float>(texture->get_quantity()));
        texture_program->set_uniform_1f(
            "number",
            static_cast<float>(texture->get_current_texture_number()));
        texture_program->set_uniform_1i("direction", direction);
        texture_program->set_uniform_1i("texture", 0);
        texture_program->set_uniform_matrix3fv(
            "Matrix", 1, GL_FALSE, matrix_first_value);

        texture->bind();
        texture->active(index);

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
    }
    void render_menu(bool& show_menu_window) final
    {
        ImGui_ImplGameEngine_NewFrame(window);

        SDL_GetWindowSize(window, &window_width, &window_height);

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
        if (ImGui::Button("Play",
                          ImVec2(menu_width - 20, menu_height / 2 - 15)))
            show_menu_window = false;
        ImGui::SetCursorPos(ImVec2(10, menu_height / 2 + 5));
        ImGui::Button("Exit", ImVec2(menu_width - 20, menu_height / 2 - 15));
        ImGui::End();

        ImGui::Render();
        ImGui_ImplGameEngine_RenderDrawData(ImGui::GetDrawData());
    }
    void clear() final
    {
        glClearColor(1.f, 1.f, 1.f, 0.f);
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
        ImGui_ImplGameEngine_Shutdown();
        delete texture_program;
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

void ImGui_ImplGameEngine_Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

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
            std::cout << mouse_pos.x << " " << mouse_pos.y << std::endl;
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

    if (!imgui_shader_program->create_shader("./vertex_shader_imgui.glsl",
                                             shader_type::vertex))
        return;

    if (!imgui_shader_program->create_shader("./fragment_shader_imgui.glsl",
                                             shader_type::fragment))
        return;

    if (!imgui_shader_program->link())
        return;

    imgui_shader_program->bind("Position", 0);
    imgui_shader_program->bind("UV", 1);
    imgui_shader_program->bind("Color", 2);

    glGenBuffers(1, &imgui_VBO);
    gl_check();
    glGenBuffers(1, &imgui_EBO);
    gl_check();

    ImGuiIO& io = ImGui::GetIO();

    int            width, height;
    unsigned char* pixels = nullptr;

    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    texture* texture = create_texture();
    io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines;
    texture->load(pixels, width, height);
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
    imgui_texture->delete_texture();
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

        SDL_Event* event;
        if (event->type != SDL_EVENT_MOUSE_BUTTON_DOWN)
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
