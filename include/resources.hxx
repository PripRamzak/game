#pragma once

#include "engine/include/sprite.hxx"

namespace resources
{
void init();

extern texture* dungeon;

extern texture* heart_full;
extern texture* heart_half;
extern texture* heart_empty;

extern sprite* warrior_idle;
extern sprite* warrior_run;
extern sprite* warrior_attack;
extern sprite* warrior_jump;
extern sprite* warrior_fall;

extern sprite* skeleton_warrior_idle;
extern sprite* skeleton_warrior_run;
extern sprite* skeleton_warrior_attack;
extern sprite* skeleton_warrior_dead;

extern sprite* skeleton_spearman_idle;
extern sprite* skeleton_spearman_walk;
extern sprite* skeleton_spearman_attack;
extern sprite* skeleton_spearman_dead;

extern sprite* skeleton_archer_idle;
extern sprite* skeleton_archer_walk;
extern sprite* skeleton_archer_attack;
extern sprite* skeleton_archer_shot;
extern sprite* skeleton_archer_dead;

extern sprite* arrow;
} // namespace resources
