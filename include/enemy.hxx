#pragma once

#include "game_object.hxx"

enum class enemy_type
{
    warrior,
    spearman
};

class enemy : public game_object
{
    virtual void add_sprite(sprite*           game_object_sprite_,
                            game_object_state state) = 0;

public:
    enemy(int               health,
          float             speed,
          float             local_pos_x,
          float             local_pos_y,
          float             global_pos_x,
          float             global_pos_y,
          float             size,
          game_object_state state);
    static void               initialize();
    virtual void              spawn()                                   = 0;
    virtual bool              is_alive()                                = 0;
    virtual bool              is_spawned()                              = 0;
    virtual void              hurt()                                    = 0;
    virtual void              move(game_object* hero)                   = 0;
    virtual void              attack(game_object* hero)                 = 0;
    virtual bool              check_hero_collision_x(game_object* hero) = 0;
    virtual bool              check_hero_collision_y(game_object* hero) = 0;
    virtual void              set_state(game_object_state state_)       = 0;
    virtual void              set_direction(int direction_)             = 0;
    virtual int               get_health()                              = 0;
    virtual float             get_current_pos_x()                       = 0;
    virtual float             get_current_pos_y()                       = 0;
    virtual float             get_move_x()                              = 0;
    virtual float             get_move_y()                              = 0;
    virtual float             get_delta_x()                             = 0;
    virtual float             get_delta_y()                             = 0;
    virtual float             get_size()                                = 0;
    virtual game_object_state get_state()                               = 0;
    virtual int               get_direction()                           = 0;
    virtual sprite*           get_sprite()                              = 0;
    virtual vertex_buffer*    get_vertex_buffer()                       = 0;
    virtual ~enemy();
};

enemy* create_enemy(int               health,
                    float             speed,
                    float             local_pos_x,
                    float             local_pos_y,
                    float             global_pos_x,
                    float             global_pos_y,
                    float             size,
                    game_object_state state,
                    enemy_type        type);