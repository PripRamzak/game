#include "engine/include/buttons.hxx"
#include "engine/include/engine.hxx"
#include "include/camera.hxx"
#include "include/enemy.hxx"
#include "include/hero.hxx"
#include "include/interface.hxx"
#include "include/map.hxx"
#include "include/resources.hxx"

#include <chrono>
#include <cstdlib>
#include <memory>
#include <thread>

#ifdef __ANDROID__
#include <SDL3/SDL_main.h>
#endif

#include <glm/gtc/type_ptr.hpp>

int main(int /*argc*/, char** /*argv*/)
{
    if (!prip_engine::init())
        return EXIT_FAILURE;

    resources::init();

    float window_width =
        static_cast<float>(prip_engine::get_window_width_pixels());
    float window_height =
        static_cast<float>(prip_engine::get_window_height_pixels());

    glm::mat4 projection =
        glm::ortho<float>(0.f, window_width, window_height, 0.f, -1.f, 1.f);

    camera* camera = create_camera(window_width / 2, window_height / 2);

    prip_engine::index_buffer* solo_objects_index_buffer =
        prip_engine::create_index_buffer();
    solo_objects_index_buffer->add_indexes(prip_engine::primitives::triangle,
                                           1);

    using namespace std::chrono_literals;

    // Map creating
    map* dungeon_map = new map(64.f, 64.f, "data/level_1.txt");

    // Warrior creating
    hero* warrior =
        new hero({ 300.f, 464.f }, 10.f, 2.f, 0, dungeon_map, 4, 12.f, 300.f);

    glm::mat4 warrior_scale =
        glm::scale(glm::mat4{ 1 },
                   glm::vec3{ warrior->get_size(), warrior->get_size(), 1.f });

    // Skeleton creating

    // std::vector<enemy*> enemies;
    /*enemy* skel = new skeleton_spearman(
        { 800.f, 418.f }, 4, 4.f, 2.f, 2000ms, 400.f, 2000ms);*/
    /*enemy* skel = new skeleton_warrior({ 800.f, 450.f }, 4, 5.f, 2.f,
     * 2000ms);*/
    enemy* skel = new skeleton_archer(
        { 800, 450.f }, 4.f, 2.f, 1, dungeon_map, 4, 2000ms);

    glm::mat4 skel_scale = glm::scale(
        glm::mat4{ 1 }, glm::vec3{ skel->get_size(), skel->get_size(), 1.f });

    // Interface

    std::vector<interface*> health;
    health.resize(2);

    health[0] = create_interface(10.f, 10.f, 64.f, 64.f);
    health[0]->add_texture(resources::heart_full);
    health[0]->add_texture(resources::heart_half);
    health[0]->add_texture(resources::heart_empty);

    health[1] = create_interface(84.f, 10.f, 64.f, 64.f);
    health[1]->add_texture(resources::heart_full);
    health[1]->add_texture(resources::heart_half);
    health[1]->add_texture(resources::heart_half);

    // Sound

    prip_engine::sound_buffer* music =
        prip_engine::create_sound_buffer("sound/dungeon_music.wav");
    music->play(prip_engine::audio_properties::looped);
    prip_engine::sound_buffer* sound_attack =
        prip_engine::create_sound_buffer("sound/attack.wav");

    bool               quit                     = false;
    bool               show_menu_window         = true;
    bool               show_in_game_menu_window = false;
    prip_engine::event event                    = prip_engine::event::released;

    prip_engine::timepoint start = prip_engine::get_time();

    while (!quit && warrior->is_alive())
    {
        // Event handling

        while (prip_engine::read_input(event))
            if (event == prip_engine::event::turn_off)
            {
                quit = true;
                break;
            }

        prip_engine::timepoint last_frame_time = prip_engine::get_time();

        std::chrono::milliseconds frame_time_dif =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                last_frame_time - start);

        if (frame_time_dif.count() < 1000 / 60)
        {
            std::this_thread::yield();
            continue;
        }
        start = last_frame_time;

        if (show_menu_window)
        {
            if (prip_engine::render_gui(show_menu_window,
                                        prip_engine::gui_type::menu))
                quit = true;
        }
        else
        {
            if (prip_engine::is_key_down(prip_engine::key::menu))
                show_in_game_menu_window = true;

            if (!show_in_game_menu_window)
            {
                if (prip_engine::is_key_down(prip_engine::key::attack) &&
                    warrior->get_state() != character_state::jump &&
                    warrior->get_state() != character_state::fall)
                {
                    warrior->set_state(character_state::melee_attack);

                    if (warrior->get_animation()->get_current_frame_number() ==
                        2)
                        sound_attack->play(prip_engine::audio_properties::once);
                }
                else if (prip_engine::is_key_down(prip_engine::key::jump) &&
                         warrior->get_state() != character_state::jump &&
                         warrior->get_state() != character_state::fall)
                    warrior->set_state(character_state::jump);
                else
                {
                    float dx = 0.f;
                    float dy = 0.f;

                    if (warrior->get_state() == character_state::jump)
                    {
                        dy--;
                        warrior->jump();
                    }
                    else if (warrior->get_state() == character_state::fall)
                        dy++;

                    if (prip_engine::is_key_down(prip_engine::key::left))
                        dx--;
                    if (prip_engine::is_key_down(prip_engine::key::right))
                        dx++;

                    if (dx == 0 && dy == 0)
                        warrior->set_state(character_state::idle);
                    else
                        warrior->move(dx, dy);
                }

                if (warrior->get_state() == character_state::melee_attack)
                    warrior->attack(skel);
                warrior->get_animation()->play(frame_time_dif);

                skel->update(warrior, frame_time_dif);
            }

            camera->look_at(warrior->get_global_pos());

            glm::mat4 view = glm::make_mat4x4(camera->get_view());

            // Map render

            glm::mat4 map_mvp = projection * view;

            /*prip_engine->render(dungeon_map->get_tileset(),
                           dungeon_map->get_vertex_buffer(map_tile_type::wall),
                           dungeon_map->get_index_buffer(map_tile_type::wall),
                           dungeon_map->get_tile_min_uv(map_tile_type::wall),
                           dungeon_map->get_tile_max_uv(map_tile_type::wall),
                           &map_mvp[0][0]);*/
            prip_engine::render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(map_tile_type::brick_top),
                dungeon_map->get_index_buffer(map_tile_type::brick_top),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_top),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_top),
                &map_mvp[0][0]);
            prip_engine::render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(map_tile_type::brick_bottom),
                dungeon_map->get_index_buffer(map_tile_type::brick_bottom),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_bottom),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_bottom),
                &map_mvp[0][0]);
            prip_engine::render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(map_tile_type::brick_left),
                dungeon_map->get_index_buffer(map_tile_type::brick_left),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_left),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_left),
                &map_mvp[0][0]);
            prip_engine::render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(map_tile_type::brick_right),
                dungeon_map->get_index_buffer(map_tile_type::brick_right),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_right),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_right),
                &map_mvp[0][0]);
            prip_engine::render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(map_tile_type::brick_top_left),
                dungeon_map->get_index_buffer(map_tile_type::brick_top_left),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_top_left),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_top_left),
                &map_mvp[0][0]);
            prip_engine::render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(map_tile_type::brick_top_right),
                dungeon_map->get_index_buffer(map_tile_type::brick_top_right),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_top_right),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_top_right),
                &map_mvp[0][0]);
            prip_engine::render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(
                    map_tile_type::brick_bottom_left),
                dungeon_map->get_index_buffer(map_tile_type::brick_bottom_left),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_bottom_left),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_bottom_left),
                &map_mvp[0][0]);
            prip_engine::render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(
                    map_tile_type::brick_bottom_right),
                dungeon_map->get_index_buffer(
                    map_tile_type::brick_bottom_right),
                dungeon_map->get_tile_min_uv(map_tile_type::brick_bottom_right),
                dungeon_map->get_tile_max_uv(map_tile_type::brick_bottom_right),
                &map_mvp[0][0]);
            prip_engine::render(
                dungeon_map->get_tileset(),
                dungeon_map->get_vertex_buffer(
                    map_tile_type::plate_bottom_right),
                dungeon_map->get_index_buffer(
                    map_tile_type::plate_bottom_right),
                dungeon_map->get_tile_min_uv(map_tile_type::plate_bottom_right),
                dungeon_map->get_tile_max_uv(map_tile_type::plate_bottom_right),
                &map_mvp[0][0]);

            // Objects render

            prip_engine::transform2d skel_pos       = skel->get_global_pos();
            glm::mat4                skel_translate = glm::translate(
                glm::mat4{ 1 }, glm::vec3{ skel_pos.x, skel_pos.y, 0.f });
            glm::mat4 skel_mvp =
                projection * view * skel_translate * skel_scale;

            prip_engine::render(skel->get_animation(),
                                solo_objects_index_buffer,
                                skel->get_direction(),
                                &skel_mvp[0][0]);

            for (auto& arrow_ :
                 dynamic_cast<skeleton_archer*>(skel)->get_arrows())
            {
                prip_engine::transform2d arrow_pos = arrow_->get_global_pos();
                glm::mat4                arrow_translate = glm::translate(
                    glm::mat4{ 1 }, glm::vec3{ arrow_pos.x, arrow_pos.y, 0.f });
                glm::mat4 arrow_scale = glm::scale(
                    glm::mat4{ 1 },
                    glm::vec3(arrow_->get_size(), arrow_->get_size(), 1.f));
                glm::mat4 arrow_mvp =
                    projection * view * arrow_translate * arrow_scale;
                prip_engine::render(arrow_->get_sprite(),
                                    solo_objects_index_buffer,
                                    arrow_->get_direction(),
                                    &arrow_mvp[0][0]);

                collision::collider* arr_coll = arrow_->get_hitbox();
                glm::mat4            arr_collider_translate = glm::translate(
                    glm::mat4{ 1 },
                    glm::vec3{ arrow_pos.x + arr_coll->get_rectangle().pos.x,
                               arrow_pos.y + arr_coll->get_rectangle().pos.y,
                               0.f });
                glm::mat4 arr_coll_mvp =
                    projection * view * arr_collider_translate;
                prip_engine::render(arr_coll->get_vertex_buffer(),
                                    arr_coll->get_index_buffer(),
                                    &arr_coll_mvp[0][0]);
            }

            if (skel->get_state() != character_state::dead)
            {
                collision::collider* coll = skel->get_collider();
                glm::mat4            skel_collider_translate = glm::translate(
                    glm::mat4{ 1 },
                    glm::vec3{ skel_pos.x + coll->get_rectangle().pos.x,
                               skel_pos.y + coll->get_rectangle().pos.y,
                               0.f });
                glm::mat4 skel_collider_mvp =
                    projection * view * skel_collider_translate;

                prip_engine::render(coll->get_vertex_buffer(),
                                    coll->get_index_buffer(),
                                    &skel_collider_mvp[0][0]);

                collision::collider* attack_trigger =
                    skel->get_attack_trigger();
                glm::mat4 skel_attack_trigger_translate = glm::translate(
                    glm::mat4{ 1 },
                    glm::vec3{
                        skel_pos.x + attack_trigger->get_rectangle().pos.x,
                        skel_pos.y + attack_trigger->get_rectangle().pos.y,
                        0.f });
                glm::mat4 skel_attack_trigger_mvp =
                    projection * view * skel_attack_trigger_translate;
                prip_engine::render(attack_trigger->get_vertex_buffer(),
                                    attack_trigger->get_index_buffer(),
                                    &skel_attack_trigger_mvp[0][0]);

                if (skel->get_state() == character_state::melee_attack)
                {
                    collision::collider* attack_coll =
                        skel->get_attack_collider();
                    glm::mat4 skel_attack_collider_translate = glm::translate(
                        glm::mat4{ 1 },
                        glm::vec3{
                            skel_pos.x + attack_coll->get_rectangle().pos.x,
                            skel_pos.y + attack_coll->get_rectangle().pos.y,
                            0.f });
                    glm::mat4 skel_attack_collider_mvp =
                        projection * view * skel_attack_collider_translate;
                    prip_engine::render(attack_coll->get_vertex_buffer(),
                                        attack_coll->get_index_buffer(),
                                        &skel_attack_collider_mvp[0][0]);
                }
            }

            prip_engine::transform2d warrior_pos = warrior->get_global_pos();
            glm::mat4                warrior_translate = glm::translate(
                glm::mat4{ 1 }, glm::vec3{ warrior_pos.x, warrior_pos.y, 0.f });

            glm::mat4 warrior_mvp =
                projection * view * warrior_translate * warrior_scale;

            prip_engine::render(warrior->get_animation(),
                                solo_objects_index_buffer,
                                warrior->get_direction(),
                                &warrior_mvp[0][0]);

            collision::collider* warrior_collider = warrior->get_collider();
            glm::mat4            warrior_collider_translate = glm::translate(
                glm::mat4{ 1 },
                glm::vec3{
                    warrior_pos.x + warrior_collider->get_rectangle().pos.x,
                    warrior_pos.y + warrior_collider->get_rectangle().pos.y,
                    0.f });
            glm::mat4 warrior_collider_mvp =
                projection * view * warrior_collider_translate;
            prip_engine::render(warrior_collider->get_vertex_buffer(),
                                warrior_collider->get_index_buffer(),
                                &warrior_collider_mvp[0][0]);

            if (warrior->get_state() == character_state::melee_attack)
            {
                collision::collider* warrior_attack_collider =
                    warrior->get_attack_collider();
                glm::mat4 warrior_attack_collider_translate = glm::translate(
                    glm::mat4{ 1 },
                    glm::vec3{
                        warrior_pos.x +
                            warrior_attack_collider->get_rectangle().pos.x,
                        warrior_pos.y +
                            warrior_attack_collider->get_rectangle().pos.y,
                        0.f });
                glm::mat4 warrior_attack_collider_mvp =
                    projection * view * warrior_attack_collider_translate;
                prip_engine::render(
                    warrior_attack_collider->get_vertex_buffer(),
                    warrior_attack_collider->get_index_buffer(),
                    &warrior_attack_collider_mvp[0][0]);
            }

            switch (warrior->get_health())
            {
                case 0:
                    health[0]->set_texture(2);
                case 1:
                    health[0]->set_texture(1);
                    break;
                case 2:
                    health[0]->set_texture(0);
                    health[1]->set_texture(2);
                    break;
                case 3:
                    health[1]->set_texture(1);
                    break;
                case 4:
                    health[1]->set_texture(0);
                    break;
                default:
                    health[1]->set_texture(0);
            }

            /*for (auto heart : health)
                prip_engine->render(heart->get_tileset(),
                               heart->get_vertex_buffer(),
                               solo_objects_index_buffer,
                               &projection[0][0]);*/

            if (!show_in_game_menu_window)
            {
#ifdef __ANDROID__
                prip_engine->render_buttons(&to_ndc_coordinates[0][0]);
#endif
            }
            else if (prip_engine::render_gui(show_in_game_menu_window,
                                             prip_engine::gui_type::menu))
                quit = true;
        }

        if (!prip_engine::swap_buffers())
        {
            prip_engine::destroy();
            return EXIT_FAILURE;
        }
    }

    prip_engine::destroy();

    return EXIT_SUCCESS;
}
