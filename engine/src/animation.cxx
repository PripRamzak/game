#include "include/animation.hxx"
#include "include/camera.hxx"

using namespace std::chrono_literals;

namespace prip_engine
{
shader_program* animation::shader = nullptr;

animation::animation(sprite*                   sprite,
                     int                       frame_quantity,
                     std::chrono::milliseconds animation_time)
    : anim_sprite(sprite)
    , frames_quantity(frame_quantity)
    , animation_time(animation_time)
    , delta_time(0ms)
{
}

void animation::play(std::chrono::milliseconds delta_time_)
{
    delta_time += delta_time_;

    if (delta_time >= animation_time)
    {
        current_frame_number++;
        if (current_frame_number == frames_quantity)
            current_frame_number = 0;

        delta_time -= animation_time;
    }
}

void animation::reset()
{
    current_frame_number = 0;
    delta_time           = 0ms;
}

void animation::draw(float* m_model)
{
    shader->use();

    shader->set_uniform_matrix4fv("model", 1, false, m_model);
    shader->set_uniform_matrix4fv("view", 1, false, camera::get_view());
    shader->set_uniform_matrix4fv(
        "projection", 1, false, camera::get_projection());

    shader->set_uniform_1f("quantity", static_cast<float>(frames_quantity));
    shader->set_uniform_1f("number", static_cast<float>(current_frame_number));

    anim_sprite->get_texture()->bind();

    anim_sprite->get_vertex_array()->bind();
    draw_elements(
        primitives::triangle,
        anim_sprite->get_vertex_array()->get_index_buffer()->get_size());
}

int animation::get_frames_quantity()
{
    return frames_quantity;
}

int animation::get_current_frame_number()
{
    return current_frame_number;
}

sprite* animation::get_sprite()
{
    return anim_sprite;
}

animation::~animation()
{
    delete anim_sprite;
}
} // namespace prip_engine