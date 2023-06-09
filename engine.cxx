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

void        ImGui_ImplGame_engine_Init();
bool        ImGui_ImplGame_engine_ProcessEvent(const SDL_Event* event);
void        ImGui_ImplGame_engine_NewFrame(SDL_Window* window);
void        ImGui_ImplGame_engine_CreateDeviceObject();
void        ImGui_ImplGame_engine_RenderDrawData(ImDrawData* draw_data);
static void ImGui_ImplGame_engine_SetClipboardText(void*, const char* text);
static const char* ImGui_ImplGame_engine_GetClipboardText(void*);

class game_engine final : public engine
{
    SDL_Window*     window;
    SDL_GLContext   context;
    shader_program* texture_program;

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

        window = SDL_CreateWindow("Game", 1280, 720, SDL_WINDOW_OPENGL);
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

        ImGui_ImplGame_engine_Init();

        return true;
    }

    bool read_input(event& event) final
    {
        SDL_Event sdl_event;

        if (SDL_PollEvent(&sdl_event))
        {
            ImGui_ImplGame_engine_ProcessEvent(&sdl_event);
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

    texture* create_triangles(texture*                  texture,
                              std::vector<triangle_2d>& texture_triangles) final
    {
        texture_program->set_uniform_1i("texture", 0);

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

    void render(const triangle_2d& triangle) final
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

        glValidateProgram(texture_program->get());
        gl_check();
        GLint validate_status = 0;
        glGetProgramiv(
            texture_program->get(), GL_VALIDATE_STATUS, &validate_status);
        gl_check();
        if (validate_status == GL_FALSE)
        {
            GLint info_length = 0;
            glGetProgramiv(
                texture_program->get(), GL_INFO_LOG_LENGTH, &info_length);
            gl_check();
            std::vector<char> info_log(static_cast<size_t>(info_length));
            glGetProgramInfoLog(
                texture_program->get(), info_length, nullptr, info_log.data());
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
        glClear(GL_COLOR_BUFFER_BIT);
        gl_check();
    }

    float get_time() final
    {
        std::uint32_t ms = SDL_GetTicks();
        return ms * 0.001f;
    }

    bool swap_buffers() final
    {
        ImGui_ImplGame_engine_NewFrame(window);
        bool show_demo_window = true;
        ImGui::ShowDemoWindow(&show_demo_window);
        ImGui::Render();
        ImGui_ImplGame_engine_RenderDrawData(ImGui::GetDrawData());
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
        texture_program->delete_program();
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

shader_program* shader_program_imgui = nullptr;
uint64_t        imgui_time           = 0;
GLuint          VBO                  = 0;
GLuint          EBO                  = 0;

void ImGui_ImplGame_engine_Init()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    io.BackendPlatformName = "custom_game_engine";

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

    io.SetClipboardTextFn = ImGui_ImplGame_engine_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplGame_engine_GetClipboardText;
    io.ClipboardUserData  = nullptr;

    ImGuiViewport* main_viewport     = ImGui::GetMainViewport();
    main_viewport->PlatformHandleRaw = nullptr;

    imgui_time = SDL_GetPerformanceCounter();
}

bool ImGui_ImplGame_engine_ProcessEvent(const SDL_Event* event)
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
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            io.AddFocusEvent(true);
            return true;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            io.AddFocusEvent(false);
            return true;
    }
    return false;
}

void ImGui_ImplGame_engine_NewFrame(SDL_Window* window)
{
    if (shader_program_imgui == nullptr)
        ImGui_ImplGame_engine_CreateDeviceObject();

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

    uint64_t frequency    = SDL_GetPerformanceFrequency();
    uint64_t current_time = SDL_GetPerformanceCounter();
    if (current_time < imgui_time)
        current_time = imgui_time + 1;

    io.DeltaTime = static_cast<float>(current_time - imgui_time) / frequency;
    imgui_time   = current_time;

    ImGui::NewFrame();
}

void ImGui_ImplGame_engine_CreateDeviceObject()
{
    shader_program_imgui = create_shader_program();

    if (!shader_program_imgui->create_shader("./vertex_shader_imgui.glsl",
                                             shader_type::vertex))
        return;

    if (!shader_program_imgui->create_shader("./fragment_shader_imgui.glsl",
                                             shader_type::fragment))
        return;

    if (!shader_program_imgui->link())
        return;

    shader_program_imgui->bind("Position", 0);
    shader_program_imgui->bind("UV", 1);
    shader_program_imgui->bind("Color", 2);

    glGenBuffers(1, &VBO);
    gl_check();
    glGenBuffers(1, &EBO);
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

void ImGui_ImplGame_engine_RenderDrawData(ImDrawData* draw_data)
{
    ImGuiIO& io = ImGui::GetIO();

    int fb_width =
        static_cast<int>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height =
        static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);

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

    shader_program_imgui->use();
    shader_program_imgui->set_uniform_1i("Texture", 0);
    shader_program_imgui->set_uniform_matrix4fv(
        "ProjMtx", 1, GL_FALSE, &ortho_projection[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    gl_check();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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
}

void ImGui_ImplGame_engine_SetClipboardText(void*, const char* text)
{
    SDL_SetClipboardText(text);
}

const char* ImGui_ImplGame_engine_GetClipboardText(void*)
{
    return SDL_GetClipboardText();
}
