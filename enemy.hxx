#pragma once

#include "game_object.hxx"
#include "hero.hxx"

class enemy : public game_object
{
    virtual void add_sprite(sprite*           game_object_sprite_,
                            game_object_state state) = 0;

public:
    enemy(int               health,
          float             local_pos_x,
          float             local_pos_y,
          float             global_pos_x,
          float             global_pos_y,
          float             size,
          game_object_state state);
    static void               initialize();
    virtual bool              is_alive()                          = 0;
    virtual void              hurt()                              = 0;
    virtual void              move(hero* hero)                    = 0;
    virtual void              attack(hero* hero)                  = 0;
    virtual bool              check_hero_collision_x(hero* hero)  = 0;
    virtual bool              check_hero_collision_y(hero* hero)  = 0;
    virtual void              set_state(game_object_state state_) = 0;
    virtual void              set_direction(int direction_)       = 0;
    virtual float             get_current_pos_x()                 = 0;
    virtual float             get_current_pos_y()                 = 0;
    virtual float             get_move_x()                        = 0;
    virtual float             get_move_y()                        = 0;
    virtual float             get_delta_x()                       = 0;
    virtual float             get_delta_y()                       = 0;
    virtual float             get_size()                          = 0;
    virtual game_object_state get_state()                         = 0;
    virtual int               get_direction()                     = 0;
    virtual sprite*           get_sprite()                        = 0;
    virtual vertex_2d*        get_vertices()                      = 0;
    virtual vertex_buffer*    get_vertex_buffer()                 = 0;
    virtual ~enemy();
};

enemy* create_enemy(int               health,
                    float             local_pos_x,
                    float             local_pos_y,
                    float             global_pos_x,
                    float             global_pos_y,
                    float             size,
                    game_object_state state);