#ifndef ROBO_CLIENT_H
#define ROBO_CLIENT_H
#include <cstdint>
#include <string>
#include <cstring>
using int32 = int32_t;
using uint32 = uint32_t;

// 也可以定义其他常用类型
using int8 = int8_t;
using uint8 = uint8_t;
using int16 = int16_t;
using uint16 = uint16_t;
using int64 = int64_t;
using uint64 = uint64_t;

// 字节类型
class Robo_Client
{
public:

        struct GameStatus {
            uint32 current_round:8;
            uint32 total_rounds ;
            uint32 red_score ;
            uint32 blue_score ;
            uint32 current_stage ; // 0:未开始,1:准备,2:自检,3:倒计时,4:比赛,5:结算
            int32 stage_countdown_sec ;
            int32 stage_elapsed_sec;
            bool is_paused ;

            uint32 current_round_() { return current_round; }
            uint32 total_rounds_() { return total_rounds; }
            uint32 red_score_() { return red_score; }
            uint32 blue_score_() { return blue_score; }
            uint32 current_stage_() { return current_stage; }
            int32 stage_countdown_sec_() { return stage_countdown_sec; }
            int32 stage_elapsed_sec_() { return stage_elapsed_sec; }
            bool is_paused_() { return is_paused; }

            void set_current_round(uint32 round) { current_round = round; }
            void set_total_rounds(uint32 rounds) { total_rounds = rounds; }
            void set_red_score(uint32 score) { red_score = score; }
            void set_blue_score(uint32 score) { blue_score = score; }
            void set_current_stage(uint32 stage) { current_stage = stage; }
            void set_stage_countdown_sec(int32 sec) { stage_countdown_sec = sec; }
            void set_stage_elapsed_sec(int32 sec) { stage_elapsed_sec = sec; }
            void set_is_paused(bool paused) { is_paused = paused; }
        };
        GameStatus  gameStatus;


public:
    Robo_Client() {}
};


//class Robo_Client
// {
// public:
//     // 2.2.1 RemoteControl
//     struct RemoteControl {
//         int32 mouse_x_ ;
//         int32 mouse_y_ ;
//         int32 mouse_z_ ;
//         bool left_button_down_ ;
//         bool right_button_down_;
//         uint32 keyboard_value_ ;
//         bool mid_button_down_ ;
//         std::string data_ = 8; // 最大30字节

//         int32 mouse_x() { return mouse_x_; }
//         int32 mouse_y() { return mouse_y_; }
//         int32 mouse_z() { return mouse_z_; }
//         bool left_button_down() { return left_button_down_; }
//         bool right_button_down() { return right_button_down_; }
//         uint32 keyboard_value() { return keyboard_value_; }
//         bool mid_button_down() { return mid_button_down_; }
//         const std::string& data() { return data_; }

//         void set_mouse_x(int32 x) { mouse_x = x; }
//         void set_mouse_y(int32 y) { mouse_y = y; }
//         void set_mouse_z(int32 z) { mouse_z = z; }
//         void set_left_button_down(bool down) { left_button_down = down; }
//         void set_right_button_down(bool down) { right_button_down = down; }
//         void set_keyboard_value(uint32 value) { keyboard_value = value; }
//         void set_mid_button_down(bool down) { mid_button_down = down; }
//         void set_data(const std::string& d) {
//             if (d.size() <= 30) data = d;
//         }
//     };

//     // 2.2.2 GameStatus
//     struct GameStatus {
//         uint32 current_round = 1;
//         uint32 total_rounds = 2;
//         uint32 red_score = 3;
//         uint32 blue_score = 4;
//         uint32 current_stage = 5; // 0:未开始,1:准备,2:自检,3:倒计时,4:比赛,5:结算
//         int32 stage_countdown_sec = 6;
//         int32 stage_elapsed_sec = 7;
//         bool is_paused = 8;

//         uint32 current_round() { return current_round; }
//         uint32 total_rounds() { return total_rounds; }
//         uint32 red_score() { return red_score; }
//         uint32 blue_score() { return blue_score; }
//         uint32 current_stage() { return current_stage; }
//         int32 stage_countdown_sec() { return stage_countdown_sec; }
//         int32 stage_elapsed_sec() { return stage_elapsed_sec; }
//         bool is_paused() { return is_paused; }

//         void set_current_round(uint32 round) { current_round = round; }
//         void set_total_rounds(uint32 rounds) { total_rounds = rounds; }
//         void set_red_score(uint32 score) { red_score = score; }
//         void set_blue_score(uint32 score) { blue_score = score; }
//         void set_current_stage(uint32 stage) { current_stage = stage; }
//         void set_stage_countdown_sec(int32 sec) { stage_countdown_sec = sec; }
//         void set_stage_elapsed_sec(int32 sec) { stage_elapsed_sec = sec; }
//         void set_is_paused(bool paused) { is_paused = paused; }
//     };

//     // 2.2.3 GlobalUnitStatus
//     struct GlobalUnitStatus {
//         uint32 base_health = 1;
//         uint32 base_status = 2; // 0:无敌,1:解除无敌未展开,2:展开
//         uint32 base_shield = 3;
//         uint32 outpost_health = 4;
//         uint32 outpost_status = 5; // 0:无敌,1:存活旋转,2:存活停转,3:被毁不可重建,4:可重建
//         std::vector<uint32> robot_health = 6;
//         std::vector<int32> robot_bullets = 7;
//         uint32 total_damage_red = 8;
//         uint32 total_damage_blue = 9;

//         uint32 base_health() { return base_health; }
//         uint32 base_status() { return base_status; }
//         uint32 base_shield() { return base_shield; }
//         uint32 outpost_health() { return outpost_health; }
//         uint32 outpost_status() { return outpost_status; }
//         const std::vector<uint32>& robot_health() { return robot_health; }
//         const std::vector<int32>& robot_bullets() { return robot_bullets; }
//         uint32 total_damage_red() { return total_damage_red; }
//         uint32 total_damage_blue() { return total_damage_blue; }

//         void set_base_health(uint32 health) { base_health = health; }
//         void set_base_status(uint32 status) { base_status = status; }
//         void set_base_shield(uint32 shield) { base_shield = shield; }
//         void set_outpost_health(uint32 health) { outpost_health = health; }
//         void set_outpost_status(uint32 status) { outpost_status = status; }
//         void set_robot_health(const std::vector<uint32>& health) { robot_health = health; }
//         void set_robot_bullets(const std::vector<int32>& bullets) { robot_bullets = bullets; }
//         void set_total_damage_red(uint32 damage) { total_damage_red = damage; }
//         void set_total_damage_blue(uint32 damage) { total_damage_blue = damage; }
//     };

//     // 2.2.4 GlobalLogisticsStatus
//     struct GlobalLogisticsStatus {
//         uint32 remaining_economy = 1;
//         uint64 total_economy_obtained = 2;
//         uint32 tech_level = 3;
//         uint32 encryption_level = 4;

//         uint32 remaining_economy() { return remaining_economy; }
//         uint64 total_economy_obtained() { return total_economy_obtained; }
//         uint32 tech_level() { return tech_level; }
//         uint32 encryption_level() { return encryption_level; }

//         void set_remaining_economy(uint32 economy) { remaining_economy = economy; }
//         void set_total_economy_obtained(uint64 economy) { total_economy_obtained = economy; }
//         void set_tech_level(uint32 level) { tech_level = level; }
//         void set_encryption_level(uint32 level) { encryption_level = level; }
//     };

//     // 2.2.5 GlobalSpecialMechanism
//     struct GlobalSpecialMechanism {
//         std::vector<uint32> mechanism_id = 1;
//         std::vector<int32> mechanism_time_sec = 2;

//         const std::vector<uint32>& mechanism_id() { return mechanism_id; }
//         const std::vector<int32>& mechanism_time_sec() { return mechanism_time_sec; }

//         void set_mechanism_id(const std::vector<uint32>& ids) { mechanism_id = ids; }
//         void set_mechanism_time_sec(const std::vector<int32>& times) { mechanism_time_sec = times; }
//     };

//     // 2.2.6 Event
//     struct Event {
//         int32 event_id = 1;
//         std::string param = 2;

//         int32 event_id() { return event_id; }
//         const std::string& param() { return param; }

//         void set_event_id(int32 id) { event_id = id; }
//         void set_param(const std::string& p) { param = p; }
//     };

//     // 2.2.7 RobotInjuryStat
//     struct RobotInjuryStat {
//         uint32 total_damage = 1;
//         uint32 collision_damage = 2;
//         uint32 small_projectile_damage = 3;
//         uint32 large_projectile_damage = 4;
//         uint32 dart_splash_damage = 5;
//         uint32 module_offline_damage = 6;
//         uint32 wifi_offline_damage = 7;
//         uint32 penalty_damage = 8;
//         uint32 server_kill_damage = 9;
//         uint32 killer_id = 10;

//         uint32 total_damage() { return total_damage; }
//         uint32 collision_damage() { return collision_damage; }
//         uint32 small_projectile_damage() { return small_projectile_damage; }
//         uint32 large_projectile_damage() { return large_projectile_damage; }
//         uint32 dart_splash_damage() { return dart_splash_damage; }
//         uint32 module_offline_damage() { return module_offline_damage; }
//         uint32 wifi_offline_damage() { return wifi_offline_damage; }
//         uint32 penalty_damage() { return penalty_damage; }
//         uint32 server_kill_damage() { return server_kill_damage; }
//         uint32 killer_id() { return killer_id; }

//         void set_total_damage(uint32 damage) { total_damage = damage; }
//         void set_collision_damage(uint32 damage) { collision_damage = damage; }
//         void set_small_projectile_damage(uint32 damage) { small_projectile_damage = damage; }
//         void set_large_projectile_damage(uint32 damage) { large_projectile_damage = damage; }
//         void set_dart_splash_damage(uint32 damage) { dart_splash_damage = damage; }
//         void set_module_offline_damage(uint32 damage) { module_offline_damage = damage; }
//         void set_wifi_offline_damage(uint32 damage) { wifi_offline_damage = damage; }
//         void set_penalty_damage(uint32 damage) { penalty_damage = damage; }
//         void set_server_kill_damage(uint32 damage) { server_kill_damage = damage; }
//         void set_killer_id(uint32 id) { killer_id = id; }
//     };

//     // 2.2.8 RobotRespawnStatus
//     struct RobotRespawnStatus {
//         bool is_pending_respawn = 1;
//         uint32 total_respawn_progress = 2;
//         uint32 current_respawn_progress = 3;
//         bool can_free_respawn = 4;
//         uint32 gold_cost_for_respawn = 5;
//         bool can_pay_for_respawn = 6;

//         bool is_pending_respawn() { return is_pending_respawn; }
//         uint32 total_respawn_progress() { return total_respawn_progress; }
//         uint32 current_respawn_progress() { return current_respawn_progress; }
//         bool can_free_respawn() { return can_free_respawn; }
//         uint32 gold_cost_for_respawn() { return gold_cost_for_respawn; }
//         bool can_pay_for_respawn() { return can_pay_for_respawn; }

//         void set_is_pending_respawn(bool pending) { is_pending_respawn = pending; }
//         void set_total_respawn_progress(uint32 progress) { total_respawn_progress = progress; }
//         void set_current_respawn_progress(uint32 progress) { current_respawn_progress = progress; }
//         void set_can_free_respawn(bool can) { can_free_respawn = can; }
//         void set_gold_cost_for_respawn(uint32 cost) { gold_cost_for_respawn = cost; }
//         void set_can_pay_for_respawn(bool can) { can_pay_for_respawn = can; }
//     };

//     // 2.2.9 RobotStaticStatus
//     struct RobotStaticStatus {
//         uint32 connection_state = 1;
//         uint32 field_state = 2;
//         uint32 alive_state = 3;
//         uint32 robot_id = 4;
//         uint32 robot_type = 5;
//         uint32 performance_system_shooter = 6; // 1:冷却优先,2:爆发优先,3:英雄近战,4:英雄远程
//         uint32 performance_system_chassis = 7; // 1:血量优先,2:功率优先,3:英雄近战,4:英雄远程
//         uint32 level = 8;
//         uint32 max_health = 9;
//         uint32 max_heat = 10;
//         float heat_cool_down_rate = 11;
//         uint32 max_power = 12;
//         uint32 max_buffer_energy = 13;
//         uint32 max_chassis_energy = 14;

//         uint32 connection_state() { return connection_state; }
//         uint32 field_state() { return field_state; }
//         uint32 alive_state() { return alive_state; }
//         uint32 robot_id() { return robot_id; }
//         uint32 robot_type() { return robot_type; }
//         uint32 performance_system_shooter() { return performance_system_shooter; }
//         uint32 performance_system_chassis() { return performance_system_chassis; }
//         uint32 level() { return level; }
//         uint32 max_health() { return max_health; }
//         uint32 max_heat() { return max_heat; }
//         float heat_cool_down_rate() { return heat_cool_down_rate; }
//         uint32 max_power() { return max_power; }
//         uint32 max_buffer_energy() { return max_buffer_energy; }
//         uint32 max_chassis_energy() { return max_chassis_energy; }

//         void set_connection_state(uint32 state) { connection_state = state; }
//         void set_field_state(uint32 state) { field_state = state; }
//         void set_alive_state(uint32 state) { alive_state = state; }
//         void set_robot_id(uint32 id) { robot_id = id; }
//         void set_robot_type(uint32 type) { robot_type = type; }
//         void set_performance_system_shooter(uint32 shooter) { performance_system_shooter = shooter; }
//         void set_performance_system_chassis(uint32 chassis) { performance_system_chassis = chassis; }
//         void set_level(uint32 l) { level = l; }
//         void set_max_health(uint32 health) { max_health = health; }
//         void set_max_heat(uint32 heat) { max_heat = heat; }
//         void set_heat_cool_down_rate(float rate) { heat_cool_down_rate = rate; }
//         void set_max_power(uint32 power) { max_power = power; }
//         void set_max_buffer_energy(uint32 energy) { max_buffer_energy = energy; }
//         void set_max_chassis_energy(uint32 energy) { max_chassis_energy = energy; }
//     };

//     // 2.2.10 RobotDynamicStatus
//     struct RobotDynamicStatus {
//         uint32 current_health = 1;
//         float current_heat = 2;
//         float last_projectile_fire_rate = 3;
//         uint32 current_chassis_energy = 4;
//         uint32 current_buffer_energy = 5;
//         uint32 current_experience = 6;
//         uint32 experience_for_upgrade = 7;
//         uint32 total_projectiles_fired = 8;
//         uint32 remaining_ammo = 9;
//         bool is_out_of_combat = 10;
//         uint32 out_of_combat_countdown = 11;
//         bool can_remote_heal = 12;
//         bool can_remote_ammo = 13;

//         uint32 current_health() { return current_health; }
//         float current_heat() { return current_heat; }
//         float last_projectile_fire_rate() { return last_projectile_fire_rate; }
//         uint32 current_chassis_energy() { return current_chassis_energy; }
//         uint32 current_buffer_energy() { return current_buffer_energy; }
//         uint32 current_experience() { return current_experience; }
//         uint32 experience_for_upgrade() { return experience_for_upgrade; }
//         uint32 total_projectiles_fired() { return total_projectiles_fired; }
//         uint32 remaining_ammo() { return remaining_ammo; }
//         bool is_out_of_combat() { return is_out_of_combat; }
//         uint32 out_of_combat_countdown() { return out_of_combat_countdown; }
//         bool can_remote_heal() { return can_remote_heal; }
//         bool can_remote_ammo() { return can_remote_ammo; }

//         void set_current_health(uint32 health) { current_health = health; }
//         void set_current_heat(float heat) { current_heat = heat; }
//         void set_last_projectile_fire_rate(float rate) { last_projectile_fire_rate = rate; }
//         void set_current_chassis_energy(uint32 energy) { current_chassis_energy = energy; }
//         void set_current_buffer_energy(uint32 energy) { current_buffer_energy = energy; }
//         void set_current_experience(uint32 exp) { current_experience = exp; }
//         void set_experience_for_upgrade(uint32 exp) { experience_for_upgrade = exp; }
//         void set_total_projectiles_fired(uint32 count) { total_projectiles_fired = count; }
//         void set_remaining_ammo(uint32 ammo) { remaining_ammo = ammo; }
//         void set_is_out_of_combat(bool out) { is_out_of_combat = out; }
//         void set_out_of_combat_countdown(uint32 countdown) { out_of_combat_countdown = countdown; }
//         void set_can_remote_heal(bool can) { can_remote_heal = can; }
//         void set_can_remote_ammo(bool can) { can_remote_ammo = can; }
//     };

//     // 2.2.11 RobotModuleStatus
//     struct RobotModuleStatus {
//         uint32 power_manager = 1;
//         uint32 rfid = 2;
//         uint32 light_strip = 3;
//         uint32 small_shooter = 4;
//         uint32 big_shooter = 5;
//         uint32 uwb = 6;
//         uint32 armor = 7;
//         uint32 video_transmission = 8;
//         uint32 capacitor = 9;
//         uint32 main_controller = 10;

//         uint32 power_manager() { return power_manager; }
//         uint32 rfid() { return rfid; }
//         uint32 light_strip() { return light_strip; }
//         uint32 small_shooter() { return small_shooter; }
//         uint32 big_shooter() { return big_shooter; }
//         uint32 uwb() { return uwb; }
//         uint32 armor() { return armor; }
//         uint32 video_transmission() { return video_transmission; }
//         uint32 capacitor() { return capacitor; }
//         uint32 main_controller() { return main_controller; }

//         void set_power_manager(uint32 status) { power_manager = status; }
//         void set_rfid(uint32 status) { rfid = status; }
//         void set_light_strip(uint32 status) { light_strip = status; }
//         void set_small_shooter(uint32 status) { small_shooter = status; }
//         void set_big_shooter(uint32 status) { big_shooter = status; }
//         void set_uwb(uint32 status) { uwb = status; }
//         void set_armor(uint32 status) { armor = status; }
//         void set_video_transmission(uint32 status) { video_transmission = status; }
//         void set_capacitor(uint32 status) { capacitor = status; }
//         void set_main_controller(uint32 status) { main_controller = status; }
//     };

//     // 2.2.12 RobotPosition
//     struct RobotPosition {
//         float x = 1;
//         float y = 2;
//         float z = 3;
//         float yaw = 4; // 正北为0度

//         float x() { return x; }
//         float y() { return y; }
//         float z() { return z; }
//         float yaw() { return yaw; }

//         void set_x(float pos_x) { x = pos_x; }
//         void set_y(float pos_y) { y = pos_y; }
//         void set_z(float pos_z) { z = pos_z; }
//         void set_yaw(float angle) { yaw = angle; }
//         void set_position(float pos_x, float pos_y, float pos_z, float angle) {
//             x = pos_x; y = pos_y; z = pos_z; yaw = angle;
//         }
//     };

//     // 2.2.13 Buff
//     struct Buff {
//         uint32 robot_id = 1;
//         uint32 buff_type = 2; // 1:攻击,2:防御,3:冷却,4:功率,5:回血,6:允许发弹,7:地形跨越预备
//         int32 buff_level = 3;
//         uint32 buff_max_time = 4;
//         uint32 buff_left_time = 5;
//         std::string msg_params = 6;

//         uint32 robot_id() { return robot_id; }
//         uint32 buff_type() { return buff_type; }
//         int32 buff_level() { return buff_level; }
//         uint32 buff_max_time() { return buff_max_time; }
//         uint32 buff_left_time() { return buff_left_time; }
//         const std::string& msg_params() { return msg_params; }

//         void set_robot_id(uint32 id) { robot_id = id; }
//         void set_buff_type(uint32 type) { buff_type = type; }
//         void set_buff_level(int32 level) { buff_level = level; }
//         void set_buff_max_time(uint32 time) { buff_max_time = time; }
//         void set_buff_left_time(uint32 time) { buff_left_time = time; }
//         void set_msg_params(const std::string& params) { msg_params = params; }
//     };

//     // 2.2.14 PenaltyInfo
//     struct PenaltyInfo {
//         uint32 penalty_type = 1; // 1:黄牌,2:双方黄牌,3:红牌,4:超功率,5:超热量,6:超射速
//         uint32 penalty_effect_sec = 2;
//         uint32 total_penalty_num = 3;

//         uint32 penalty_type() { return penalty_type; }
//         uint32 penalty_effect_sec() { return penalty_effect_sec; }
//         uint32 total_penalty_num() { return total_penalty_num; }

//         void set_penalty_type(uint32 type) { penalty_type = type; }
//         void set_penalty_effect_sec(uint32 sec) { penalty_effect_sec = sec; }
//         void set_total_penalty_num(uint32 num) { total_penalty_num = num; }
//     };

//     // 2.2.15 RobotPathPlanInfo
//     struct RobotPathPlanInfo {
//         uint32 intention = 1; // 1:攻击,2:防守,3:移动
//         uint32 start_pos_x = 2; // 分米
//         uint32 start_pos_y = 3;
//         std::vector<int32> offset_x = 4; // 长度49, -128~+127
//         std::vector<int32> offset_y = 5;
//         uint32 sender_id = 6;

//         uint32 intention() { return intention; }
//         uint32 start_pos_x() { return start_pos_x; }
//         uint32 start_pos_y() { return start_pos_y; }
//         const std::vector<int32>& offset_x() { return offset_x; }
//         const std::vector<int32>& offset_y() { return offset_y; }
//         uint32 sender_id() { return sender_id; }

//         void set_intention(uint32 intent) { intention = intent; }
//         void set_start_pos_x(uint32 x) { start_pos_x = x; }
//         void set_start_pos_y(uint32 y) { start_pos_y = y; }
//         void set_offset_x(const std::vector<int32>& offsets) { offset_x = offsets; }
//         void set_offset_y(const std::vector<int32>& offsets) { offset_y = offsets; }
//         void set_sender_id(uint32 id) { sender_id = id; }
//     };

//     // 2.2.16 MapClickInfoNotify
//     struct MapClickInfoNotify {
//         uint32 is_send_all = 1; // 0:指定客户端,1:除哨兵,2:含哨兵
//         std::array<uint8_t, 7> robot_id = {0}; // 固定7字节
//         uint32 mode = 3; // 1:攻击,2:防御,3:警戒,4:自定义
//         uint32 enemy_id = 4;
//         uint32 ascii = 5;
//         uint32 type = 6; // 1:地图,2:对方机器人
//         uint32 screen_x = 7;
//         uint32 screen_y = 8;
//         float map_x = 9;
//         float map_y = 10;

//         uint32 is_send_all() { return is_send_all; }
//         const std::array<uint8_t, 7>& robot_id() { return robot_id; }
//         uint32 mode() { return mode; }
//         uint32 enemy_id() { return enemy_id; }
//         uint32 ascii() { return ascii; }
//         uint32 type() { return type; }
//         uint32 screen_x() { return screen_x; }
//         uint32 screen_y() { return screen_y; }
//         float map_x() { return map_x; }
//         float map_y() { return map_y; }

//         void set_is_send_all(uint32 send_all) { is_send_all = send_all; }
//         void set_robot_id(const std::array<uint8_t, 7>& id) { robot_id = id; }
//         void set_mode(uint32 m) { mode = m; }
//         void set_enemy_id(uint32 id) { enemy_id = id; }
//         void set_ascii(uint32 a) { ascii = a; }
//         void set_type(uint32 t) { type = t; }
//         void set_screen_x(uint32 x) { screen_x = x; }
//         void set_screen_y(uint32 y) { screen_y = y; }
//         void set_map_x(float x) { map_x = x; }
//         void set_map_y(float y) { map_y = y; }
//     };

//     // 2.2.17 RaderInfoToClient
//     struct RaderInfoToClient {
//         uint32 target_robot_id = 1;
//         float target_pos_x = 2;
//         float target_pos_y = 3;
//         float forward_angle = 4;
//         uint32 is_high_light = 5; // 0:否,1:是

//         uint32 target_robot_id() { return target_robot_id; }
//         float target_pos_x() { return target_pos_x; }
//         float target_pos_y() { return target_pos_y; }
//         float forward_angle() { return forward_angle; }
//         uint32 is_high_light() { return is_high_light; }

//         void set_target_robot_id(uint32 id) { target_robot_id = id; }
//         void set_target_pos_x(float x) { target_pos_x = x; }
//         void set_target_pos_y(float y) { target_pos_y = y; }
//         void set_forward_angle(float angle) { forward_angle = angle; }
//         void set_is_high_light(uint32 highlight) { is_high_light = highlight; }
//     };

//     // 2.2.18 CustomByteBlock
//     struct CustomByteBlock {
//         std::string data = 1; // 最大1.2kbit

//         const std::string& data() { return data; }

//         void set_data(const std::string& d) {
//             if (d.size() <= 150) data = d; // 1.2kbit = 150字节
//         }
//     };

//     // 2.2.19 AssemblyCommand
//     struct AssemblyCommand {
//         uint32 operation = 1; // 1:确认装配,2:取消装配
//         uint32 difficulty = 2;

//         uint32 operation() { return operation; }
//         uint32 difficulty() { return difficulty; }

//         void set_operation(uint32 op) { operation = op; }
//         void set_difficulty(uint32 diff) { difficulty = diff; }
//     };

//     // 2.2.20 TechCoreMotionStateSync
//     struct TechCoreMotionStateSync {
//         uint32 maximum_difficulty_level = 1;
//         uint32 status = 2; // 1:未进入装配,2:移动中,3:可首个步骤,4:可下个步骤,5:全部完成,6:确认后移动

//         uint32 maximum_difficulty_level() { return maximum_difficulty_level; }
//         uint32 status() { return status; }

//         void set_maximum_difficulty_level(uint32 level) { maximum_difficulty_level = level; }
//         void set_status(uint32 s) { status = s; }
//     };

//     // 2.2.21 RobotPerformanceSelectionCommand
//     struct RobotPerformanceSelectionCommand {
//         uint32 shooter = 1;
//         uint32 chassis = 2;

//         uint32 shooter() { return shooter; }
//         uint32 chassis() { return chassis; }

//         void set_shooter(uint32 s) { shooter = s; }
//         void set_chassis(uint32 c) { chassis = c; }
//     };

//     // 2.2.22 RobotPerformanceSelectionSync
//     struct RobotPerformanceSelectionSync {
//         uint32 shooter = 1;
//         uint32 chassis = 2;

//         uint32 shooter() { return shooter; }
//         uint32 chassis() { return chassis; }

//         void set_shooter(uint32 s) { shooter = s; }
//         void set_chassis(uint32 c) { chassis = c; }
//     };

//     // 2.2.23 HeroDeployModeEventCommand
//     struct HeroDeployModeEventCommand {
//         uint32 mode = 1; // 0:退出,1:进入

//         uint32 mode() { return mode; }

//         void set_mode(uint32 m) { mode = m; }
//     };

//     // 2.2.24 DeployModeStatusSync
//     struct DeployModeStatusSync {
//         uint32 status = 1; // 0:未部署,1:已部署

//         uint32 status() { return status; }

//         void set_status(uint32 s) { status = s; }
//     };

//     // 2.2.25 RuneActivateCommand
//     struct RuneActivateCommand {
//         uint32 activate = 1; // 1:开启

//         uint32 activate() { return activate; }

//         void set_activate(uint32 a) { activate = a; }
//     };

//     // 2.2.26 RuneStatusSync
//     struct RuneStatusSync {
//         uint32 rune_status = 1; // 1:未激活,2:正在激活,3:已激活
//         uint32 activated_arms = 2;
//         uint32 average_rings = 3;

//         uint32 rune_status() { return rune_status; }
//         uint32 activated_arms() { return activated_arms; }
//         uint32 average_rings() { return average_rings; }

//         void set_rune_status(uint32 status) { rune_status = status; }
//         void set_activated_arms(uint32 arms) { activated_arms = arms; }
//         void set_average_rings(uint32 rings) { average_rings = rings; }
//     };

//     // 2.2.27 SentinelStatusSync
//     struct SentinelStatusSync {
//         uint32 posture_id = 1; // 1:进攻,2:防御,3:移动
//         bool is_weakened = 2;

//         uint32 posture_id() { return posture_id; }
//         bool is_weakened() { return is_weakened; }

//         void set_posture_id(uint32 id) { posture_id = id; }
//         void set_is_weakened(bool weakened) { is_weakened = weakened; }
//     };

//     // 2.2.28 DartCommand
//     struct DartCommand {
//         uint32 target_id = 1; // 1:前哨站,2:基地固定,3:基地随机固定,4:基地随机移动,5:基地末端移动
//         bool open = 2;

//         uint32 target_id() { return target_id; }
//         bool open() { return open; }

//         void set_target_id(uint32 id) { target_id = id; }
//         void set_open(bool o) { open = o; }
//     };

//     // 2.2.29 DartSelectTargetStatusSync
//     struct DartSelectTargetStatusSync {
//         uint32 target_id = 1;
//         bool open = 2;

//         uint32 target_id() { return target_id; }
//         bool open() { return open; }

//         void set_target_id(uint32 id) { target_id = id; }
//         void set_open(bool o) { open = o; }
//     };

//     // 2.2.30 GuardCtrlCommand
//     struct GuardCtrlCommand {
//         uint32 command_id = 1; // 1~10

//         uint32 command_id() { return command_id; }

//         void set_command_id(uint32 id) { command_id = id; }
//     };

//     // 2.2.31 GuardCtrlResult
//     struct GuardCtrlResult {
//         uint32 command_id = 1;
//         uint32 result_code = 2; // 0:成功

//         uint32 command_id() { return command_id; }
//         uint32 result_code() { return result_code; }

//         void set_command_id(uint32 id) { command_id = id; }
//         void set_result_code(uint32 code) { result_code = code; }
//     };

//     // 2.2.32 AirSupportCommand
//     struct AirSupportCommand {
//         uint32 command_id = 1; // 1:免费呼叫,2:金币呼叫,3:中断

//         uint32 command_id() { return command_id; }

//         void set_command_id(uint32 id) { command_id = id; }
//     };

//     // 2.2.33 AirSupportStatusSync
//     struct AirSupportStatusSync {
//         uint32 airsupport_status = 1; // 0:未进行,1:进行中,2:被锁定
//         uint32 left_time = 2;
//         uint32 cost_coins = 3;

//         uint32 airsupport_status() { return airsupport_status; }
//         uint32 left_time() { return left_time; }
//         uint32 cost_coins() { return cost_coins; }

//         void set_airsupport_status(uint32 status) { airsupport_status = status; }
//         void set_left_time(uint32 time) { left_time = time; }
//         void set_cost_coins(uint32 coins) { cost_coins = coins; }
//     };

// public:
//     Robo_Client() {}
// };

#endif // ROBO_CLIENT_H
