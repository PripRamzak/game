#pragma once

#include "engine/include/collision.hxx"
#include "engine/include/geometry.hxx"
#include "engine/include/shader_program.hxx"
#include "engine/include/texture.hxx"
#include "engine/include/vertex_array.hxx"

class map
{
public:
    map(std::string file_path, int tile_size);
    void                                       draw();
    prip_engine::texture*                      get_tileset();
    const std::vector<prip_engine::collider*>& get_colliders();
    const std::vector<prip_engine::trigger*>&  get_triggers();
    prip_engine::transform2d                   get_spawn_pos();
    ~map();

private:
    struct layer
    {
        struct object
        {
            object(prip_engine::transform2d pos, int tile_id);

            prip_engine::transform2d pos;
            int                      tile_id;
        };

        std::string                name;
        prip_engine::vertex_array* vao = nullptr;
        std::vector<object>        objects;
    };

    prip_engine::texture*               tileset = nullptr;
    prip_engine::shader_program*        shader  = nullptr;
    std::vector<layer>                  layers;
    std::vector<prip_engine::collider*> colliders;
    std::vector<prip_engine::trigger*>  triggers;
    prip_engine::transform2d            spawn_pos;
    int                                 tile_size = 0;
};