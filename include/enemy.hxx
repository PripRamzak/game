#pragma once

#include "arrow.hxx"
#include "hero.hxx"

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
          hero*                     target,
          std::chrono::milliseconds attack_delay);
    void draw() override;
    virtual ~enemy();

protected:
    virtual void move();
    void         attack(std::chrono::milliseconds delta_time);
    void         is_damaged(std::chrono::milliseconds delta_time);

    hero*                     target;
    bool                      damaged;
    std::chrono::milliseconds invulnerability_cooldown;
    std::chrono::milliseconds invulnerability_cooldown_dt;
    prip_engine::collider*      attack_trigger;
    std::chrono::milliseconds attack_delay;
    std::chrono::milliseconds attack_delay_dt;
};

class skeleton_warrior : public enemy
{
public:
    skeleton_warrior(prip_engine::transform2d global_pos,
                     int                      direction,
                     map*                     level_map,
                     hero*                    target);
    void update(std::chrono::milliseconds delta_time) override;
    void draw() override;
    ~skeleton_warrior();

private:
    bool                 agro = false;
    prip_engine::collider* agro_trigger;
};

class skeleton_spearman : public enemy
{
public:
    skeleton_spearman(prip_engine::transform2d global_pos,
                      int                      direction,
                      map*                     level_map,
                      hero*                    target);
    void update(std::chrono::milliseconds delta_time) override;
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
    skeleton_archer(prip_engine::transform2d global_pos,
                    int                      direction,
                    map*                     level_map,
                    hero*                    target);
    void update(std::chrono::milliseconds delta_time) override;
    void draw() override;
    ~skeleton_archer();

private:
    void shoot(game_object* hero, std::chrono::milliseconds delta_time);

    std::vector<std::unique_ptr<arrow>> arrows;
    prip_engine::collider*                shot_trigger;
    std::chrono::milliseconds           shot_cooldown;
    std::chrono::milliseconds           shot_cooldown_dt;
};