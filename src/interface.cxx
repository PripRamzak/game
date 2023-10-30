#include "include/interface.hxx"

#include <vector>

class game_interface : public interface
{
    int                   current_texture = 0;
    std::vector<texture*> textures;
    vertex_buffer*        vertex_buffer_;

public:
    game_interface(float pos_x, float pos_y, int width, int height)
    {
        vertex_2d v1 = { pos_x, pos_y, 0.f, 1.f };
        vertex_2d v2 = { pos_x + width, pos_y, 1.f, 1.f };
        vertex_2d v3 = { pos_x + width, pos_y + height, 1.f, 0.f };
        vertex_2d v4 = { pos_x, pos_y + height, 0.f, 0.f };

        vertex_buffer_ = create_vertex_buffer();
        vertex_buffer_->add_vertex(v1);
        vertex_buffer_->add_vertex(v2);
        vertex_buffer_->add_vertex(v3);
        vertex_buffer_->add_vertex(v4);
        vertex_buffer_->buffer_data();
    }
    void add_texture(texture* texture_) final { textures.push_back(texture_); }
    void set_texture(int number) final { current_texture = number; }
    texture*       get_texture() final { return textures[current_texture]; }
    vertex_buffer* get_vertex_buffer() final { return vertex_buffer_; }
};

interface::~interface() = default;

interface* create_interface(float pos_x, float pos_y, int width, int height)
{
    return new game_interface(pos_x, pos_y, width, height);
}