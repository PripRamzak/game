#include "camera.hxx"

#include <glm/ext/matrix_transform.hpp>
#include <glm/matrix.hpp>

class game_camera : public camera
{
    glm::vec3 position{ 0.f, 0.f, 2.f };
    glm::vec3 target{ 0.f, 0.f, 0.f };
    glm::vec3 up{ 0.f, 1.f, 0.f };
    glm::mat4 view{ 1 };

public:
    void look_at(float pos_x, float pos_y) final
    {
        position = glm::vec3(pos_x, pos_y, 2.f);
        target   = glm::vec3(pos_x, pos_y, 0.f);
        view     = glm::lookAt(position, target, up);
    }
    float* get_view() final { return &view[0][0]; }
};

camera* create_camera()
{
    return new game_camera;
}