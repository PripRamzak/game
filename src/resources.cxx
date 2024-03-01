#include "include/resources.hxx"

#include <iostream>

namespace resources
{
texture* dungeon{ nullptr };

texture* heart_full{ nullptr };
texture* heart_half{ nullptr };
texture* heart_empty{ nullptr };

sprite* warrior_idle{ nullptr };
sprite* warrior_run{ nullptr };
sprite* warrior_attack{ nullptr };
sprite* warrior_jump{ nullptr };
sprite* warrior_fall{ nullptr };

sprite* skeleton_warrior_idle{ nullptr };
sprite* skeleton_warrior_run{ nullptr };
sprite* skeleton_warrior_attack{ nullptr };
sprite* skeleton_warrior_dead{ nullptr };

sprite* skeleton_spearman_idle{ nullptr };
sprite* skeleton_spearman_walk{ nullptr };
sprite* skeleton_spearman_attack{ nullptr };
sprite* skeleton_spearman_dead{ nullptr };

sprite* skeleton_archer_idle{ nullptr };
sprite* skeleton_archer_walk{ nullptr };
sprite* skeleton_archer_attack{ nullptr };
sprite* skeleton_archer_shot{ nullptr };
sprite* skeleton_archer_dead{ nullptr };

sprite* arrow{ nullptr };

static bool res_init = false;

void init()
{
    if (res_init)
    {
        std::cout << "Resources already init" << std::endl;
        return;
    }

    dungeon = create_texture("img/map/dungeon.png");

    heart_full  = create_texture("img/interface/heart_full.png");
    heart_half  = create_texture("img/interface/heart_half.png");
    heart_empty = create_texture("img/interface/heart_empty.png");

    texture* t_warrior_idle   = create_texture("img/hero/warrior_idle.png");
    texture* t_warrior_run    = create_texture("img/hero/warrior_run.png");
    texture* t_warrior_attack = create_texture("img/hero/warrior_attack.png");
    texture* t_warrior_jump   = create_texture("img/hero/warrior_jump.png");
    texture* t_warrior_fall   = create_texture("img/hero/warrior_fall.png");

    texture* t_skeleton_warrior_idle =
        create_texture("img/enemy/skeleton_warrior_idle.png");
    texture* t_skeleton_warrior_run =
        create_texture("img/enemy/skeleton_warrior_run.png");
    texture* t_skeleton_warrior_attack =
        create_texture("img/enemy/skeleton_warrior_attack.png");
    texture* t_skeleton_warrior_dead =
        create_texture("img/enemy/skeleton_warrior_dead.png");

    texture* t_skeleton_spearman_idle =
        create_texture("img/enemy/skeleton_spearman_idle.png");
    texture* t_skeleton_spearman_walk =
        create_texture("img/enemy/skeleton_spearman_walk.png");
    texture* t_skeleton_spearman_attack =
        create_texture("img/enemy/skeleton_spearman_attack.png");
    texture* t_skeleton_spearman_dead =
        create_texture("img/enemy/skeleton_spearman_dead.png");

    texture* t_skeleton_archer_idle =
        create_texture("img/enemy/skeleton_archer_idle.png");
    texture* t_skeleton_archer_walk =
        create_texture("img/enemy/skeleton_archer_walk.png");
    texture* t_skeleton_archer_attack =
        create_texture("img/enemy/skeleton_archer_attack.png");
    texture* t_skeleton_archer_shot =
        create_texture("img/enemy/skeleton_archer_shot.png");
    texture* t_skeleton_archer_dead =
        create_texture("img/enemy/skeleton_archer_dead.png");

    texture* t_arrow = create_texture("img/enemy/arrow.png");

    warrior_idle   = new sprite(t_warrior_idle, { 48.f, 48.f });
    warrior_run    = new sprite(t_warrior_run, { 48.f, 48.f });
    warrior_attack = new sprite(t_warrior_attack, { 86.f, 48.f });
    warrior_jump   = new sprite(t_warrior_jump, { 48.f, 48.f });
    warrior_fall   = new sprite(t_warrior_fall, { 48.f, 48.f });

    skeleton_warrior_idle = new sprite(t_skeleton_warrior_idle, { 58.f, 64.f });
    skeleton_warrior_run  = new sprite(t_skeleton_warrior_run, { 70.f, 64.f });
    skeleton_warrior_attack =
        new sprite(t_skeleton_warrior_attack, { 114.f, 76.f }, { 57.f, 44.f });
    skeleton_warrior_dead =
        new sprite(t_skeleton_warrior_dead, { 128.f, 64.f });

    skeleton_spearman_idle =
        new sprite(t_skeleton_spearman_idle, { 36.f, 82.f }, { 18.f, 35.f });
    skeleton_spearman_walk =
        new sprite(t_skeleton_spearman_walk, { 64.f, 94.f });
    skeleton_spearman_attack =
        new sprite(t_skeleton_spearman_attack, { 100.f, 56.f }, { 50.f, 9.f });
    skeleton_spearman_dead =
        new sprite(t_skeleton_spearman_dead, { 128.f, 82.f }, { 64.f, 35.f });

    skeleton_archer_idle = new sprite(t_skeleton_archer_idle, { 128.f, 64.f });
    skeleton_archer_walk = new sprite(t_skeleton_archer_walk, { 128.f, 64.f });
    skeleton_archer_attack =
        new sprite(t_skeleton_archer_attack, { 128.f, 66.f }, { 64.f, 34.f });
    skeleton_archer_shot =
        new sprite(t_skeleton_archer_shot, { 128.f, 74.f }, { 64.f, 42.f });
    skeleton_archer_dead = new sprite(t_skeleton_archer_dead, { 128.f, 64.f });

    arrow = new sprite(t_arrow, { 48, 16 });

    res_init = true;
}
} // namespace resources