#include "include/resources.hxx"

#include <iostream>

namespace resources
{
prip_engine::texture* dungeon{ nullptr };

prip_engine::texture* heart_full{ nullptr };
prip_engine::texture* heart_half{ nullptr };
prip_engine::texture* heart_empty{ nullptr };

prip_engine::sprite* hero_idle{ nullptr };
prip_engine::sprite* hero_run{ nullptr };
prip_engine::sprite* hero_attack{ nullptr };
prip_engine::sprite* hero_jump{ nullptr };
prip_engine::sprite* hero_fall{ nullptr };
prip_engine::sprite* hero_dead{ nullptr };

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

prip_engine::sound_buffer* main_theme{ nullptr };
prip_engine::sound_buffer* warrior_attack_sound{ nullptr };

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
        prip_engine::create_texture("img/hero/idle.png");
    prip_engine::texture* t_warrior_run =
        prip_engine::create_texture("img/hero/run.png");
    prip_engine::texture* t_warrior_attack =
        prip_engine::create_texture("img/hero/attack.png");
    prip_engine::texture* t_warrior_jump =
        prip_engine::create_texture("img/hero/jump.png");
    prip_engine::texture* t_warrior_fall =
        prip_engine::create_texture("img/hero/fall.png");
    prip_engine::texture* t_warrior_dead =
        prip_engine::create_texture("img/hero/dead.png");

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

    hero_idle =
        new prip_engine::sprite(t_warrior_idle,
                                { 32.f, 40.f },
                                { 2.f, 0.f },
                                { 40.f / t_warrior_idle->get_width(), 0.f },
                                { 72.f / t_warrior_idle->get_width(), 0.5f });
    hero_run =
        new prip_engine::sprite(t_warrior_run,
                                { 32.f, 40.f },
                                { 0.f, 0.f },
                                { 40.f / t_warrior_run->get_width(), 0.f },
                                { 72.f / t_warrior_run->get_width(), 0.5f });
    hero_attack = new prip_engine::sprite(
        t_warrior_attack,
        { 88.f, 40.f },
        { 10.f, 0.f },
        { 24.f / t_warrior_attack->get_width(), 0.f },
        { 112.f / t_warrior_attack->get_width(), 0.5f });
    hero_jump =
        new prip_engine::sprite(t_warrior_jump,
                                { 32.f, 40.f },
                                { 0.f, 0.f },
                                { 40.f / t_warrior_jump->get_width(), 0.f },
                                { 72.f / t_warrior_jump->get_width(), 0.5f });
    hero_fall =
        new prip_engine::sprite(t_warrior_fall,
                                { 32.f, 40.f },
                                { 0.f, 0.f },
                                { 40.f / t_warrior_fall->get_width(), 0.f },
                                { 72.f / t_warrior_fall->get_width(), 0.5f });
    hero_dead =
        new prip_engine::sprite(t_warrior_dead,
                                { 32.f, 40.f },
                                { 2.f, 0.f },
                                { 40.f / t_warrior_idle->get_width(), 0.f },
                                { 72.f / t_warrior_idle->get_width(), 0.5f });

    skeleton_warrior_idle = new prip_engine::sprite(
        t_skeleton_warrior_idle,
        { 58.f, 64.f },
        { 0.f, 0.f },
        { 0.f, 0.f },
        { 58.f / t_skeleton_warrior_idle->get_width(), 1.f });
    skeleton_warrior_run = new prip_engine::sprite(
        t_skeleton_warrior_run,
        { 70.f, 64.f },
        { 0.f, 0.f },
        { 0.f, 0.f },
        { 70.f / t_skeleton_warrior_run->get_width(), 1.f });
    skeleton_warrior_attack = new prip_engine::sprite(
        t_skeleton_warrior_attack,
        { 114.f, 76.f },
        { 0.f, -6.f },
        { 0.f, 0.f },
        { 114.f / t_skeleton_warrior_attack->get_width(), 1.f });
    skeleton_warrior_dead = new prip_engine::sprite(
        t_skeleton_warrior_dead,
        { 128.f, 64.f },
        { 0.f, 0.f },
        { 0.f, 0.f },
        { 128.f / t_skeleton_warrior_dead->get_width(), 1.f });

    skeleton_spearman_idle = new prip_engine::sprite(
        t_skeleton_spearman_idle,
        { 36.f, 82.f },
        { 0.f, 6.f },
        { 0.f, 0.f },
        { 36.f / t_skeleton_spearman_idle->get_width(), 1.f });
    skeleton_spearman_walk = new prip_engine::sprite(
        t_skeleton_spearman_walk,
        { 64.f, 94.f },
        { 0.f, 0.f },
        { 0.f, 0.f },
        { 64.f / t_skeleton_spearman_walk->get_width(), 1.f });
    skeleton_spearman_attack = new prip_engine::sprite(
        t_skeleton_spearman_attack,
        { 100.f, 56.f },
        { 0.f, 19.f },
        { 0.f, 0.f },
        { 100.f / t_skeleton_spearman_attack->get_width(), 1.f });
    skeleton_spearman_dead = new prip_engine::sprite(
        t_skeleton_spearman_dead,
        { 128.f, 82.f },
        { 0.f, -6.f },
        { 0.f, 0.f },
        { 128.f / t_skeleton_spearman_dead->get_width(), 1.f });

    skeleton_archer_idle = new prip_engine::sprite(
        t_skeleton_archer_idle,
        { 128.f, 64.f },
        { 0.f, 0.f },
        { 0.f, 0.f },
        { 128.f / t_skeleton_archer_idle->get_width(), 1.f });
    skeleton_archer_walk = new prip_engine::sprite(
        t_skeleton_archer_walk,
        { 128.f, 64.f },
        { 0.f, 0.f },
        { 0.f, 0.f },
        { 128.f / t_skeleton_archer_walk->get_width(), 1.f });
    skeleton_archer_attack = new prip_engine::sprite(
        t_skeleton_archer_attack,
        { 128.f, 66.f },
        { 0.f, -1.f },
        { 0.f, 0.f },
        { 128.f / t_skeleton_archer_attack->get_width(), 1.f });
    skeleton_archer_shot = new prip_engine::sprite(
        t_skeleton_archer_shot,
        { 128.f, 74.f },
        { 0.f, -5.f },
        { 0.f, 0.f },
        { 128.f / t_skeleton_archer_shot->get_width(), 1.f });
    skeleton_archer_dead = new prip_engine::sprite(
        t_skeleton_archer_dead,
        { 128.f, 64.f },
        { 0.f, 0.f },
        { 0.f, 0.f },
        { 128.f / t_skeleton_archer_dead->get_width(), 1.f });

    arrow = new prip_engine::sprite(t_arrow, { 48.f, 16.f });

    main_theme = prip_engine::create_sound_buffer("sound/dungeon_music.wav");
    warrior_attack_sound = prip_engine::create_sound_buffer("sound/attack.wav");

    res_init = true;
}
} // namespace resources