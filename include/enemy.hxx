#pragma once

#include "arrow.hxx"
#include "character.hxx"

class enemy : public character
{
public:
    enemy(prip_engine::transform2d  global_pos,
          float                     speed,
          float                     size,
          int                       direction,
          map*                      level_map,
          int                       health,
          character_state           state,
          std::chrono::milliseconds attack_delay);
    void draw(float* matrix) override;
    virtual ~enemy();

protected:
    virtual void move();
    void         attack(character* hero, std::chrono::milliseconds);

    collision::collider*      attack_trigger;
    std::chrono::milliseconds attack_delay;
    std::chrono::milliseconds attack_delay_dt;
};

class skeleton_warrior : public enemy
{
public:
    skeleton_warrior(prip_engine::transform2d  global_pos,
                     float                     speed,
                     float                     size,
                     int                       direction,
                     map*                      level_map,
                     int                       health,
                     std::chrono::milliseconds attack_delay);
    void update(std::chrono::milliseconds delta_time, character* hero) override;
    void draw(float* matrix) override;
    ~skeleton_warrior();

private:
    bool                 agro = false;
    collision::collider* agro_trigger;
};

class skeleton_spearman : public enemy
{
public:
    skeleton_spearman(prip_engine::transform2d  global_pos,
                      float                     speed,
                      float                     size,
                      int                       direction,
                      map*                      level_map,
                      int                       health,
                      std::chrono::milliseconds attack_delay,
                      float                     patrol_area,
                      std::chrono::milliseconds patrol_time);
    void update(std::chrono::milliseconds delta_time, character* hero) override;
    ~skeleton_spearman();

private:
    void move() override;

    int                       patrol_direction = 0;
    float                     patrol_area      = 0.f;
    float                     patrol_area_dt   = 0.f;
    std::chrono::milliseconds patrol_time;
    std::chrono::milliseconds patrol_time_dt;
};

class skeleton_archer : public enemy
{
public:
    skeleton_archer(prip_engine::transform2d  global_pos,
                    float                     speed,
                    float                     size,
                    int                       direction,
                    map*                      level_map,
                    int                       health,
                    std::chrono::milliseconds attack_delay);
    void update(std::chrono::milliseconds delta_time, character* hero) override;
    void draw(float* matrix) override;
    ~skeleton_archer();

private:
    void shoot(game_object* hero, std::chrono::milliseconds delta_time);

    std::vector<std::unique_ptr<arrow>> arrows;
    collision::collider*                shot_trigger;
    std::chrono::milliseconds           shot_cooldown;
    std::chrono::milliseconds           shot_cooldown_dt;
};