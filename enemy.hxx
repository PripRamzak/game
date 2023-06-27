#pragma once

#include "game_object.hxx"
#include "hero.hxx"

class enemy : public game_object
{
public:
    enemy(float             local_pos_x,
          float             local_pos_y,
          float             global_pos_x,
          float             global_pos_y,
          float             size,
          game_object_state state);
    virtual void  add_sprite(sprite*           game_object_sprite_,
                             game_object_state state)                       = 0;
    virtual float test1()                                                   = 0;
    virtual float test2()                                                   = 0;
    virtual void  move(hero* hero, float window_width, float window_height) = 0;
    virtual bool  check_hero_x_collision(hero* hero,
                                         float window_width,
                                         float window_height)               = 0;
    virtual void  set_state(game_object_state state_)                       = 0;
    virtual void  set_direction(int direction_)                             = 0;
    virtual float get_current_pos_x()                                       = 0;
    virtual float get_current_pos_y()                                       = 0;
    virtual void  get_delta_pos(float& x, float& y)                         = 0;
    virtual float get_size()                                                = 0;
    virtual game_object_state get_state()                                   = 0;
    virtual int               get_direction()                               = 0;
    virtual sprite*           get_sprite()                                  = 0;
    virtual vertex_2d*        get_vertices()                                = 0;
    virtual vertex_buffer*    get_vertex_buffer()                           = 0;
    virtual ~enemy();
};

enemy* create_enemy(float             local_pos_x,
                    float             local_pos_y,
                    float             global_pos_x,
                    float             global_pos_y,
                    float             size,
                    game_object_state state);