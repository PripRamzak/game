#pragma once

#include "game_object.hxx"

class enemy : public game_object
{
public:
    enemy(transform2d               global_pos,
          int                       health,
          float                     speed,
          float                     size,
          game_object_state         state,
          std::chrono::milliseconds attack_delay);
    static void          initialize();
    void                 spawn();
    bool                 is_spawned();
    virtual void         update(game_object*              hero,
                                std::chrono::milliseconds delta_time) = 0;
    collision::collider* get_attack_trigger();
    ~enemy();

protected:
    virtual void move();
    void         attack(game_object* hero, std::chrono::milliseconds);

    collision::collider*      attack_trigger;
    std::chrono::milliseconds attack_delay;
    std::chrono::milliseconds attack_delay_dt;
    bool                      spawned = false;
};

class skeleton_warrior : public enemy
{
public:
    skeleton_warrior(transform2d               global_pos,
                     int                       health,
                     float                     speed,
                     float                     size,
                     std::chrono::milliseconds attack_delay,
                     float                     agro_area);
    void update(game_object*              hero,
                std::chrono::milliseconds delta_time) override;

private:
    bool  agro      = false;
    float agro_area = 0.f;
};

class skeleton_spearman : public enemy
{
public:
    skeleton_spearman(transform2d               global_pos,
                      int                       health,
                      float                     speed,
                      float                     size,
                      std::chrono::milliseconds attack_delay,
                      float                     patrol_area,
                      std::chrono::milliseconds patrol_time);
    void update(game_object*              hero,
                std::chrono::milliseconds delta_time) override;

private:
    void move() override;

    int                       patrol_direction = 0;
    float                     patrol_area      = 0.f;
    float                     patrol_area_dt   = 0.f;
    std::chrono::milliseconds patrol_time;
    std::chrono::milliseconds patrol_time_dt;
};