#include "sprite.hxx"

#include <iostream>
#include <vector>

#include <glm/matrix.hpp>

class game_sprite final : public sprite
{
    std::vector<texture*> textures;
    vertex_2d             vertices[4];
    glm::vec3             position;
    int                   current_texture = 0;
    float                 delta_pos_x     = 0.f;
    float                 delta_pos_y     = 0.f;

public:
    game_sprite(float pos_x, float pos_y, float width, float height)
    {

        vertex_2d v1 = { pos_x - width / 2.f, pos_y - height / 2.f, 0.f, 1.f };
        vertex_2d v2 = { pos_x + width / 2.f, pos_y - height / 2.f, 1.f, 1.f };
        vertex_2d v3 = { pos_x + width / 2.f, pos_y + height / 2.f, 1.f, 0.f };
        vertex_2d v4 = { pos_x - width / 2.f, pos_y + height / 2.f, 0.f, 0.f };

        vertices[0] = v1;
        vertices[1] = v2;
        vertices[2] = v3;
        vertices[3] = v4;

        position = { (v1.x + v3.x) / 2.f, (v1.y + v3.y) / 2.f, 1.f };
    }
    void add_texture(texture* texture) final { textures.push_back(texture); }
    void set_current_texture(int current_texture_) final
    {
        this->current_texture = current_texture_;
    }
    void next_sprite() final { textures[current_texture]->next_texture(); }
    void move(float x, float y) final
    {
        delta_pos_x += x;
        delta_pos_y += y;
        std::cout << delta_pos_x << " " << delta_pos_y << std::endl;
    }
    float      get_current_pos_x() final { return position.x + delta_pos_x; }
    float      get_current_pos_y() final { return position.y + delta_pos_y; }
    vertex_2d* get_vertices() final { return vertices; }
    texture*   get_sprite() final { return textures[current_texture]; }
};

sprite::~sprite() = default;

sprite* create_sprite(float pos_x, float pos_y, float width, float height)
{
    return new game_sprite(pos_x, pos_y, width, height);
}