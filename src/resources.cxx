#include "include/resources.hxx"

#include <iostream>

namespace resources
{
prip_engine::texture* dungeon{ nullptr };

prip_engine::texture* heart_full{ nullptr };
prip_engine::texture* heart_half{ nullptr };
prip_engine::texture* heart_empty{ nullptr };

prip_engine::sprite* warrior_idle{ nullptr };
prip_engine::sprite* warrior_run{ nullptr };
prip_engine::sprite* warrior_attack{ nullptr };
prip_engine::sprite* warrior_jump{ nullptr };
prip_engine::sprite* warrior_fall{ nullptr };

prip_engine::sprite* skeleton_warrior_idle{ nullptr };
prip_engine::sprite* skeleton_warrior_run{ nullptr };
prip_engine::sprite* skeleton_warrior_attack{ nullptr };
prip_engine::sprite* skeleton_warrior_dead{ nullptr };

prip_engine::sprite* skeleton_spearman_idle{ nullptr };
prip_engine::sprite* skeleton_spearman_walk{ nullptr };
prip_engine::sprite* skeleton_spearman_attack{ nullptr };
prip_engine::sprite* skeleton_spearman_dead{ nullptr };

prip_engine::sprite* skeleton_archer_idle{ nullptr };
prip_engine::sprite* skeleton_archer_walk{ nullptr };
prip_engine::sprite* skeleton_archer_attack{ nullptr };
prip_engine::sprite* skeleton_archer_shot{ nullptr };
prip_engine::sprite* skeleton_archer_dead{ nullptr };

prip_engine::sprite* arrow{ nullptr };

static bool res_init = false;

void init()
{
    if (res_init)
    {
        std::cout << "Resources already init" << std::endl;
        return;
    }

    dungeon = prip_engine::create_texture("img/map/dungeon.png");

    heart_full  = prip_engine::create_texture("img/interface/heart_full.png");
    heart_half  = prip_engine::create_texture("img/interface/heart_half.png");
    heart_empty = prip_engine::create_texture("img/interface/heart_empty.png");

    prip_engine::texture* t_warrior_idle =
        prip_engine::create_texture("img/hero/warrior_idle.png");
    prip_engine::texture* t_warrior_run =
        prip_engine::create_texture("img/hero/warrior_run.png");
    prip_engine::texture* t_warrior_attack =
        prip_engine::create_texture("img/hero/warrior_attack.png");
    prip_engine::texture* t_warrior_jump =
        prip_engine::create_texture("img/hero/warrior_jump.png");
    prip_engine::texture* t_warrior_fall =
        prip_engine::create_texture("img/hero/warrior_fall.png");

    prip_engine::texture* t_skeleton_warrior_idle =
        prip_engine::create_texture("img/enemy/skeleton_warrior_idle.png");
    prip_engine::texture* t_skeleton_warrior_run =
        prip_engine::create_texture("img/enemy/skeleton_warrior_run.png");
    prip_engine::texture* t_skeleton_warrior_attack =
        prip_engine::create_texture("img/enemy/skeleton_warrior_attack.png");
    prip_engine::texture* t_skeleton_warrior_dead =
        prip_engine::create_texture("img/enemy/skeleton_warrior_dead.png");

    prip_engine::texture* t_skeleton_spearman_idle =
        prip_engine::create_texture("img/enemy/skeleton_spearman_idle.png");
    prip_engine::texture* t_skeleton_spearman_walk =
        prip_engine::create_texture("img/enemy/skeleton_spearman_walk.png");
    prip_engine::texture* t_skeleton_spearman_attack =
        prip_engine::create_texture("img/enemy/skeleton_spearman_attack.png");
    prip_engine::texture* t_skeleton_spearman_dead =
        prip_engine::create_texture("img/enemy/skeleton_spearman_dead.png");

    prip_engine::texture* t_skeleton_archer_idle =
        prip_engine::create_texture("img/enemy/skeleton_archer_idle.png");
    prip_engine::texture* t_skeleton_archer_walk =
        prip_engine::create_texture("img/enemy/skeleton_archer_walk.png");
    prip_engine::texture* t_skeleton_archer_attack =
        prip_engine::create_texture("img/enemy/skeleton_archer_attack.png");
    prip_engine::texture* t_skeleton_archer_shot =
        prip_engine::create_texture("img/enemy/skeleton_archer_shot.png");
    prip_engine::texture* t_skeleton_archer_dead =
        prip_engine::create_texture("img/enemy/skeleton_archer_dead.png");

    prip_engine::texture* t_arrow =
        prip_engine::create_texture("img/enemy/arrow.png");

    warrior_idle   = new prip_engine::sprite(t_warrior_idle, { 48.f, 48.f });
    warrior_run    = new prip_engine::sprite(t_warrior_run, { 48.f, 48.f });
    warrior_attack = new prip_engine::sprite(t_warrior_attack, { 86.f, 48.f });
    warrior_jump   = new prip_engine::sprite(t_warrior_jump, { 48.f, 48.f });
    warrior_fall   = new prip_engine::sprite(t_warrior_fall, { 48.f, 48.f });

    skeleton_warrior_idle =
        new prip_engine::sprite(t_skeleton_warrior_idle, { 58.f, 64.f });
    skeleton_warrior_run =
        new prip_engine::sprite(t_skeleton_warrior_run, { 70.f, 64.f });
    skeleton_warrior_attack = new prip_engine::sprite(
        t_skeleton_warrior_attack, { 114.f, 76.f }, { 57.f, 44.f });
    skeleton_warrior_dead =
        new prip_engine::sprite(t_skeleton_warrior_dead, { 128.f, 64.f });

    skeleton_spearman_idle = new prip_engine::sprite(
        t_skeleton_spearman_idle, { 36.f, 82.f }, { 18.f, 35.f });
    skeleton_spearman_walk =
        new prip_engine::sprite(t_skeleton_spearman_walk, { 64.f, 94.f });
    skeleton_spearman_attack = new prip_engine::sprite(
        t_skeleton_spearman_attack, { 100.f, 56.f }, { 50.f, 9.f });
    skeleton_spearman_dead = new prip_engine::sprite(
        t_skeleton_spearman_dead, { 128.f, 82.f }, { 64.f, 35.f });

    skeleton_archer_idle =
        new prip_engine::sprite(t_skeleton_archer_idle, { 128.f, 64.f });
    skeleton_archer_walk =
        new prip_engine::sprite(t_skeleton_archer_walk, { 128.f, 64.f });
    skeleton_archer_attack = new prip_engine::sprite(
        t_skeleton_archer_attack, { 128.f, 66.f }, { 64.f, 34.f });
    skeleton_archer_shot = new prip_engine::sprite(
        t_skeleton_archer_shot, { 128.f, 74.f }, { 64.f, 42.f });
    skeleton_archer_dead =
        new prip_engine::sprite(t_skeleton_archer_dead, { 128.f, 64.f });

    arrow = new prip_engine::sprite(t_arrow, { 48, 16 });

    res_init = true;
}
} // namespace resources