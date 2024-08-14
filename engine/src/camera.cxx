#include "include/camera.hxx"

#include "glm/gtc/type_ptr.hpp"

namespace prip_engine
{
namespace camera
{
static float camera_width  = 0.f;
static float camera_height = 0.f;

static glm::vec3 position{ 0.f, 0.f, 2.f };
static glm::vec3 target{ 0.f, 0.f, 0.f };
static glm::vec3 up{ 0.f, 1.f, 0.f };

static glm::mat4 view{ 1 };
static glm::mat4 projection{ 1 };

void init(float width, float height)
{
    camera_width  = width / 2;
    camera_height = height / 2;

    projection = glm::ortho<float>(0.f, width, height, 0.f, -5.f, 5.f);
}

void look_at(prip_engine::transform2d pos)
{
    position =
        glm::vec3(pos.x - camera_width, pos.y - camera_height * 1.25f, 2.f);
    target =
        glm::vec3(pos.x - camera_width, pos.y - camera_height * 1.25f, 0.f);
    view = glm::lookAt(position, target, up);
}
float* get_view()
{
    return &view[0][0];
}

float* get_projection()
{
    return &projection[0][0];
}
}; // namespace camera
} // namespace prip_engine