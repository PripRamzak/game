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
        vertex2d_uv vertices[4];
        vertices[0] = { pos_x, pos_y, 0.f, 1.f };
        vertices[1] = { pos_x + width, pos_y, 1.f, 1.f };
        vertices[2] = { pos_x + width, pos_y + height, 1.f, 0.f };
        vertices[3] = { pos_x, pos_y + height, 0.f, 0.f };

        vertex_buffer_ = create_vertex_buffer();
        vertex_buffer_->buffer_data(vertices, static_cast<size_t>(4));
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