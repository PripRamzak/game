#include "include/camera.hxx"

#include <glm/ext/matrix_transform.hpp>

class game_camera : public camera
{
    float width;
    float height;

    glm::vec3 position{ 0.f, 0.f, 2.f };
    glm::vec3 target{ 0.f, 0.f, 0.f };
    glm::vec3 up{ 0.f, 1.f, 0.f };
    glm::mat4 view{ 1 };

public:
    game_camera(float width_, float height_)
        : width(width_)
        , height(height_)
    {
    }
    void look_at(float pos_x, float pos_y) final
    {
        position = glm::vec3(pos_x - width, pos_y - height, 2.f);
        target   = glm::vec3(pos_x - width, pos_y - height, 0.f);
        view     = glm::lookAt(position, target, up);
    }
    float* get_view() final { return &view[0][0]; }
};

camera* create_camera(float width, float height)
{
    return new game_camera(width, height);
}