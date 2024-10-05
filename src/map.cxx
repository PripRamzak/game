#include "engine/include/camera.hxx"
#include "engine/include/engine.hxx"
#include "engine/include/memory_buf.hxx"
#include "engine/include/types.hxx"

#include "include/map.hxx"
#include "include/resources.hxx"

#include <algorithm>
#include <iostream>
#include <bitset>

#include <tinyxml2.h>

map::map(std::string file_path, int tile_size)
    : tileset(resources::dungeon)
    , tile_size(tile_size)
{
    shader = prip_engine::create_shader_program();

    if (!shader->create_shader("shaders/map.vs",
                               prip_engine::shader_type::vertex))
        throw std::runtime_error("Failed to create map");

    if (!shader->create_shader("shaders/map.gs",
                               prip_engine::shader_type::geometry))
        throw std::runtime_error("Failed to create map");

    if (!shader->create_shader("shaders/map.fs",
                               prip_engine::shader_type::fragment))
        throw std::runtime_error("Failed to create map");

    if (!shader->link())
        throw std::runtime_error("Failed to create map");

    prip_engine::memory_buf buf_level = prip_engine::load_file(file_path);
    std::istream            data_level(&buf_level);
    std::stringstream       ss;
    ss << data_level.rdbuf();

    std::string level_xml = ss.str();

    tinyxml2::XMLDocument level_doc;
    if (level_doc.Parse(level_xml.c_str(), level_xml.length()) != 0)
        throw std::runtime_error("Failed to read level file");

    tinyxml2::XMLElement* element_map = level_doc.RootElement();
    int                   width, height;
    width  = element_map->IntAttribute("width");
    height = element_map->IntAttribute("height");

    prip_engine::transform2d tile_sz{ element_map->FloatAttribute("tilewidth"),
                                      element_map->FloatAttribute(
                                          "tileheight") };
    prip_engine::transform2d tile_scale{ tile_size / tile_sz.x,
                                         tile_size / tile_sz.y };

    tinyxml2::XMLElement* element_tileset =
        element_map->FirstChildElement("tileset");
    int gid = element_tileset->IntAttribute("firstgid");

    tinyxml2::XMLElement* element_layer =
        element_map->FirstChildElement("layer");

    if (!element_layer)
        throw std::runtime_error("Failed to read level file");

    while (element_layer->FirstChildElement("data"))
    {
        layer layer;
        layer.name = element_layer->Name();

        tinyxml2::XMLElement* element_data;
        element_data = element_layer->FirstChildElement("data");

        std::string tile_ids = element_data->GetText();
        std::replace_if(
            tile_ids.begin(),
            tile_ids.end(),
            [](const char c) { return c == ','; },
            ' ');
        std::stringstream stream_number(tile_ids);

        for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++)
            {
                int tile_id;
                stream_number >> tile_id;
                if (tile_id > 0)
                {
                    layer::object obj{ { static_cast<float>(x * tile_size),
                                         static_cast<float>(y * tile_size) },
                                       tile_id - gid };
                    layer.objects.push_back(obj);
                }
            }

        prip_engine::vertex_buffer* vbo = prip_engine::create_vertex_buffer();
        layer.vao = prip_engine::create_vertex_array(vbo, nullptr);
        layer.vao->bind();

        vbo->buffer_data(layer.objects.data(),
                         layer.objects.size() * sizeof(layer::object));

        vbo->set_attrib_pointer(
            0, 2, prip_engine::t_float, false, sizeof(layer::object), 0);
        vbo->set_attrib_ipointer(1,
                                 1,
                                 prip_engine::t_int,
                                 sizeof(layer::object),
                                 offsetof(layer::object, tile_id));

        layers.push_back(layer);

        element_layer = element_layer->NextSiblingElement();
    }

    tinyxml2::XMLElement* element_object_group =
        element_map->FirstChildElement("objectgroup");

    if (!element_object_group)
        throw std::runtime_error("Failed to read level file");

    while (element_object_group)
    {
        tinyxml2::XMLElement* element_object =
            element_object_group->FirstChildElement("object");

        std::string name = element_object_group->Attribute("name");

        if (strcmp(element_object_group->Attribute("name"), "collision") == 0)
        {
            while (element_object)
            {
                prip_engine::transform2d collider_pos{
                    element_object->FloatAttribute("x") * tile_scale.x,
                    element_object->FloatAttribute("y") * tile_scale.y
                };
                prip_engine::transform2d collider_size{
                    element_object->FloatAttribute("width") * tile_scale.x,
                    element_object->FloatAttribute("height") * tile_scale.y
                };

                if (strcmp(element_object->Attribute("name"), "collider") == 0)
                    colliders.push_back(new prip_engine::collider(
                        collider_pos,
                        { 0.f, 0.f },
                        collider_size,
                        { prip_engine::e_color::GREEN, 0.6f },
                        1.f,
                        0,
                        false));
                else
                {
                    tinyxml2::XMLElement* element_properties =
                        element_object->FirstChildElement();
                    tinyxml2::XMLElement* element_property =
                        element_properties->FirstChildElement("property");
                    std::string type = element_property->Attribute("value");

                    prip_engine::trigger::type t;
                    if (type == "damage")
                        t = prip_engine::trigger::type::damage;

                    triggers.push_back(new prip_engine::trigger(
                        collider_pos,
                        { 0.f, 0.f },
                        collider_size,
                        { prip_engine::e_color::YELLOW, 0.6f },
                        1.f,
                        0,
                        false,
                        t));
                }

                element_object = element_object->NextSiblingElement();
            }
        }
        else
            spawn_pos = { element_object->FloatAttribute("x") * tile_scale.x,
                          element_object->FloatAttribute("y") * tile_scale.y };

        element_object_group = element_object_group->NextSiblingElement();
    }
}

void map::draw()
{
    shader->use();
    shader->set_uniform_matrix4fv(
        "view", 1, false, prip_engine::camera::get_view());
    shader->set_uniform_matrix4fv(
        "projection", 1, false, prip_engine::camera::get_projection());
    shader->set_uniform_1i("tile_size", tile_size);
    shader->set_uniform_2i(
        "texture_size", tileset->get_width(), tileset->get_height());

    tileset->bind();

    for (auto layer : layers)
    {
        layer.vao->bind();
        prip_engine::draw_arrays(
            prip_engine::primitives::point, 0, layer.objects.size());
    }

    for (auto collider : colliders)
        collider->draw();
}

prip_engine::texture* map::get_tileset()
{
    return tileset;
}

const std::vector<prip_engine::collider*>& map::get_colliders()
{
    return colliders;
}

const std::vector<prip_engine::trigger*>& map::get_triggers()
{
    return triggers;
}

prip_engine::transform2d map::get_spawn_pos()
{
    return spawn_pos;
}

map::layer::object::object(prip_engine::transform2d pos, int tile_id)
    : pos(pos)
    , tile_id(tile_id)
{
}

map::~map()
{

    for (auto layer : layers)
        delete layer.vao;
    for (auto collider : colliders)
        delete collider;
    delete shader;
}