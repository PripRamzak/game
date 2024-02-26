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

sprite* skeleton_spearman_idle{ nullptr };
sprite* skeleton_spearman_walk{ nullptr };
sprite* skeleton_spearman_attack{ nullptr };

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

    texture* t_skeleton_spearman_idle =
        create_texture("img/enemy/skeleton_spearman_idle.png");
    texture* t_skeleton_spearman_walk =
        create_texture("img/enemy/skeleton_spearman_walk.png");
    texture* t_skeleton_spearman_attack =
        create_texture("img/enemy/skeleton_spearman_attack.png");

    warrior_idle   = new sprite(t_warrior_idle, { 48.f, 48.f });
    warrior_run    = new sprite(t_warrior_run, { 48.f, 48.f });
    warrior_attack = new sprite(t_warrior_attack, { 86.f, 48.f });
    warrior_jump   = new sprite(t_warrior_jump, { 48.f, 48.f });
    warrior_fall   = new sprite(t_warrior_fall, { 48.f, 48.f });

    skeleton_warrior_idle = new sprite(t_skeleton_warrior_idle, { 58.f, 64.f });
    skeleton_warrior_run  = new sprite(t_skeleton_warrior_run, { 70.f, 64.f });
    skeleton_warrior_attack =
        new sprite(t_skeleton_warrior_attack, { 114.f, 76.f }, { 57.f, 44.f });

    skeleton_spearman_idle =
        new sprite(t_skeleton_spearman_idle, { 36.f, 82.f }, { 18.f, 54.f });
    skeleton_spearman_walk =
        new sprite(t_skeleton_spearman_walk, { 64.f, 94.f }, { 32.f, 66.f });
    skeleton_spearman_attack =
        new sprite(t_skeleton_spearman_attack, { 100.f, 56.f });

    res_init = true;
}
} // namespace resources