#include "aim.hpp"

#include "../sdk/convars.hpp"
#include "../sdk/engine.hpp"
#include "../sdk/input_system.hpp"
#include "../utilities/math.hpp"
#include "../utilities/remote.hpp"

#include "../dependencies/lazy_importer/lazy_importer.hpp"

extern "C" {
#define WINAPI __stdcall
#define DECLSPEC_IMPORT __declspec(dllimport)
	DECLSPEC_IMPORT void WINAPI mouse_event(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uintptr_t);
}

namespace cheat::features {
	float aimbot_fov = 2.0f / 180.0f;
	float aimbot_smooth = 4.5f;
	int bone_list[6] = { 5, 4, 3, 0, 7, 8 };
	sdk::entity target;
	int current_tick, previous_tick, target_bone;

	typedef struct {
		int bone;
		float radius;
		sdk::vec3 min;
		sdk::vec3 max;
	} HITBOX;

	HITBOX hitbox_list[2][6] = {
		{
			{5, 6.200000f, {4.800000f,  0.150000f,  -4.100000f}, {4.800000f,  0.150000f, 4.100000f}},
			{4, 6.500000f, {3.800000f,  0.800000f,  -2.400000f}, {3.800000f,  0.400000f, 2.400000f}},
			{3, 6.000000f, {1.400000f,  0.800000f,  3.100000f},  {1.400000f,  0.800000f, -3.100000f}},
			{0, 6.000000f, {-2.700000f, 1.100000f,  -3.200000f}, {-2.700000f, 1.100000f, 3.200000f}},
			{7, 3.500000f, {-1.300000f, -0.200000f, 0.000000f},  {1.400000f,  0.600000f, 0.000000f}},
			{8, 4.300000f, {-1.100000f, 1.400000f,  0.100000f},  {3.000000f,  0.800000f, 0.100000f}}
		},
		{
			{5, 6.200000f, {4.800000f,  0.150000f,  -4.100000f}, {4.800000f,  0.150000f, 4.100000f}},
			{4, 6.500000f, {3.800000f,  0.800000f,  -2.400000f}, {3.800000f,  0.400000f, 2.400000f}},
			{3, 6.000000f, {1.400000f,  0.800000f,  3.100000f},  {1.400000f,  0.800000f, -3.100000f}},
			{0, 6.000000f, {-2.700000f, 1.100000f,  -3.200000f}, {-2.700000f, 1.100000f, 3.200000f}},
			{7, 3.500000f, {-1.300000f, 0.900000f,  0.000000f},  {1.400000f,  1.300000f, 0.000000f}},
			{8, 3.200000f, {-0.200000f, 1.100000f,  0.000000f},  {3.600000f,  0.100000f, 0.000000f}}
		}
	};

	float get_fov(sdk::vec3 vangle, sdk::vec3 angle) {
		sdk::vec3 a0, a1;

		math::angle_vec(vangle, &a0);
		math::angle_vec(angle, &a1);

		return RAD2DEG(acos(math::vec_dot(a0, a1) / math::vec_length(a0)));
	}

	sdk::vec3 get_target_angle(sdk::entity self, sdk::entity target, int bone_id) {
		sdk::matrix3x4_t m;
		sdk::vec3 c, p;

		target.get_bone_matrix(bone_id, &m);
		c = self.eye_pos;

		m[0][3] -= c.x, m[1][3] -= c.y, m[2][3] -= c.z;
		c.x = m[0][3], c.y = m[1][3], c.z = m[2][3];

		math::vec_normalize(&c);
		math::vec_angles(c, &c);

		if (self.shots_fired > 1) {
			p = self.punch;
			c.x -= p.x * 2.0f, c.y -= p.y * 2.0f, c.z -= p.z * 2.0f;
		}

		math::vec_clamp(&c);

		return c;
	}

	bool get_target(sdk::entity self, sdk::vec3 vangle, bool mp_teammates_are_enemies) {
		float best_fov, fov;

		best_fov = 9999.0f;

		for (auto& entity : sdk::entities) {
			if (!entity.is_valid()) {
				continue;
			}

			if (!mp_teammates_are_enemies && self.team == entity.team) {
				continue;
			}

			for (int bone : bone_list) {
				fov = get_fov(vangle, get_target_angle(self, entity, bone));

				if (fov < best_fov) {
					best_fov = fov;
					target = entity;
					target_bone = bone;
				}
			}
		}

		return best_fov != 9999.0f;
	}

	void mouse_move(int x, int y) {
		LI_FN(mouse_event).safe_cached()(0x0001, x, y, 0, 0);
	}

	void mouse1_down() {
		LI_FN(mouse_event).safe_cached()(0x0002, 0, 0, 0, 0);
	}

	void mouse1_up() {
		LI_FN(mouse_event).safe_cached()(0x0004, 0, 0, 0, 0);
	}

	void aim_at_target(sdk::vec3 vangle, sdk::vec3 angle, float sensitivity) {
		float x, y, sx, sy;

		y = vangle.x - angle.x;
		if (y > 89.0f) {
			y = 89.0f;
		}
		else if (y < -89.0f) {
			y = -89.0f;
		}

		x = vangle.y - angle.y;
		if (x > 180.0f) {
			x -= 360.0f;
		}
		else if (x < -180.0f) {
			x += 360.0f;
		}

		if (fabs(x) / 180.0f >= aimbot_fov) {
			return;
		}
		if (fabs(y) / 89.0f >= aimbot_fov) {
			return;
		}

		x = (x / sensitivity) / 0.022f;
		y = (y / sensitivity) / -0.022f;

		if (aimbot_smooth) {
			sx = 0.0f, sy = 0.0f;

			if (sx < x) {
				sx += 1.0f + (x / aimbot_smooth);
			}
			else if (sx > x) {
				sx -= 1.0f - (x / aimbot_smooth);
			}

			if (sy < y) {
				sy += 1.0f + (y / aimbot_smooth);
			}
			else if (sy > y) {
				sy -= 1.0f - (y / aimbot_smooth);
			}
		}
		else {
			sx = x, sy = y;
		}

		if (current_tick - previous_tick > 0) {
			previous_tick = current_tick;
			mouse_move(static_cast<int>(sx), static_cast<int>(sy));
		}
	}

	int crosshair_id(sdk::vec3 vangle, sdk::entity self, bool mp_teammates_are_enemies) {
		sdk::vec3 dir, eye;
		int i, id, j;
		sdk::matrix3x4_t matrix;
		bool status;

		i = 1;
		dir = math::vec_atd(vangle);
		eye = self.eye_pos;

		for (auto& entity : sdk::entities) {
			if (!entity.is_valid()) {
				continue;
			}

			id = entity.team;

			if (!mp_teammates_are_enemies && self.team == id) {
				continue;
			}

			id -= 2;

			for (j = 6; j--;) {
				entity.get_bone_matrix(hitbox_list[id][j].bone, &matrix);
				status = math::vec_min_max(eye, dir, math::vec_transform(hitbox_list[id][j].min, matrix), math::vec_transform(hitbox_list[id][j].max, matrix), hitbox_list[id][j].radius);

				if (status) {
					return i;
				}
			}

			i++;
		}

		return 0;
	}

	void triggerbot(sdk::entity local_player, sdk::vec3 vangle, bool mp_teammates_are_enemies) {
		if (sdk::input_system::is_button_down(sdk::input_system::MOUSE_5)) {
			if (crosshair_id(vangle, local_player, mp_teammates_are_enemies)) {
				mouse1_down();
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				mouse1_up();
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		}
	}

	void aim_assist(sdk::entity local_player, sdk::vec3 vangle, bool mp_teammates_are_enemies) {
		float sensitivity = sdk::cvar::sensitivity.get_float();

		current_tick = local_player.tick_count;

		if (local_player.scoped) {
			sensitivity *= (local_player.fov / 90.0f);
		}

		if (sdk::input_system::is_button_down(sdk::input_system::MOUSE_1)) {
			if (!target.is_valid() && !get_target(local_player, vangle, mp_teammates_are_enemies)) {
				return;
			}

			aim_at_target(vangle, get_target_angle(local_player, target, target_bone), sensitivity);
		}
		else {
			target = {};
		}
	}

	void aim(std::atomic<bool>& active) {
		while (active) {
			if (!sdk::engine::is_in_game())
				continue;

			if (!sdk::entity_mutex.try_lock_shared()) {
				continue;
			}

			static auto local_player = sdk::get_client_entity(sdk::engine::get_local_player());
			
			static int mp_teammates_are_enemies = sdk::cvar::mp_teammates_are_enemies.get_int();

			sdk::vec3 vangle = sdk::engine::get_view_angles();

			aim_assist(local_player, vangle, mp_teammates_are_enemies);
			triggerbot(local_player, vangle, mp_teammates_are_enemies);

			sdk::entity_mutex.unlock_shared();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}
