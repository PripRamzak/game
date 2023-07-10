#pragma once

#include "game_object.hxx"
#include "map.hxx"

class hero : public game_object
{
    virtual void add_sprite(sprite*           game_object_sprite_,
                            game_object_state state) = 0;

public:
    hero(int               health,
         float             local_pos_x,
         float             local_pos_y,
         float             global_pos_x,
         float             global_pos_y,
         float             size,
         game_object_state state);
    static void   initialize();
    virtual bool  is_alive()                                          = 0;
    virtual void  hurt()                                              = 0;
    virtual void  move(float    delta_x_,
                       float    delta_y_,
                       map*     map,
                       map_tile type,
                       bool     skeleton_collision = false)               = 0;
    virtual void  attack(game_object* enemy, bool skeleton_collision) = 0;
    virtual bool  check_collision_map(map* map, map_tile type)        = 0;
    virtual void  set_state(game_object_state state_)                 = 0;
    virtual void  set_direction(int direction_)                       = 0;
    virtual float get_current_pos_x()                                 = 0;
    virtual float get_current_pos_y()                                 = 0;
    virtual float get_delta_x()                                       = 0;
    virtual float get_delta_y()                                       = 0;
    virtual float get_size()                                          = 0;
    virtual game_object_state get_state()                             = 0;
    virtual int               get_direction()                         = 0;
    virtual sprite*           get_sprite()                            = 0;
    virtual vertex_2d*        get_vertices()                          = 0;
    virtual vertex_buffer*    get_vertex_buffer()                     = 0;
    virtual ~hero();
};

hero* create_hero(int               health,
                  float             local_pos_x,
                  float             local_pos_y,
                  float             global_pos_x,
                  float             global_pos_y,
                  float             size,
                  game_object_state state);