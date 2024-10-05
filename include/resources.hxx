#pragma once

#include "engine/include/sound_buffer.hxx"
#include "engine/include/sprite.hxx"

namespace resources
{
void init();

extern prip_engine::texture* dungeon;

extern prip_engine::texture* heart_full;
extern prip_engine::texture* heart_half;
extern prip_engine::texture* heart_empty;

extern prip_engine::sprite* hero_idle;
extern prip_engine::sprite* hero_run;
extern prip_engine::sprite* hero_attack;
extern prip_engine::sprite* hero_jump;
extern prip_engine::sprite* hero_fall;
extern prip_engine::sprite* hero_dead;

extern prip_engine::sprite* skeleton_warrior_idle;
extern prip_engine::sprite* skeleton_warrior_run;
extern prip_engine::sprite* skeleton_warrior_attack;
extern prip_engine::sprite* skeleton_warrior_dead;

extern prip_engine::sprite* skeleton_spearman_idle;
extern prip_engine::sprite* skeleton_spearman_walk;
extern prip_engine::sprite* skeleton_spearman_attack;
extern prip_engine::sprite* skeleton_spearman_dead;

extern prip_engine::sprite* skeleton_archer_idle;
extern prip_engine::sprite* skeleton_archer_walk;
extern prip_engine::sprite* skeleton_archer_attack;
extern prip_engine::sprite* skeleton_archer_shot;
extern prip_engine::sprite* skeleton_archer_dead;

extern prip_engine::sprite* arrow;

extern prip_engine::sound_buffer* main_theme;
extern prip_engine::sound_buffer* warrior_attack_sound;
} // namespace resources
