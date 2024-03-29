#pragma once

#include "engine.hpp"
#include "netvars.hpp"
#include "offsets.hpp"
#include "types.hpp"
#include "../utilities/remote.hpp"

#include <array>
#include <atomic>
#include <shared_mutex>
#include <thread>
#include <vector>

namespace cheat::sdk {
	struct entity {
		ptr_t address;
		std::array<std::uint8_t, 0xAA50> chunk;

		int team;
		int health;
		int life_state;
		int tick_count;
		int shots_fired;
		bool scoped;
		bool dormant;
		vec3 origin;
		vec3 view_offset;
		vec3 eye_pos;
		vec3 velocity;
		vec3 punch;
		int fov;

		entity() {
			update();
		}

		entity(const entity& entity) {
			address = entity.address;
			update();
		}

		void update() {
			if (!address) {
				return;
			}

			remote::read(address, chunk.data(), chunk.size());

			team = *reinterpret_cast<int*>(chunk.data() + netvars::m_iTeamNum);
			health = *reinterpret_cast<int*>(chunk.data() + netvars::m_iHealth);
			life_state = *reinterpret_cast<int*>(chunk.data() + netvars::m_lifeState);
			tick_count = *reinterpret_cast<int*>(chunk.data() + netvars::m_nTickBase);
			shots_fired = *reinterpret_cast<int*>(chunk.data() + netvars::m_iShotsFired);
			scoped = *reinterpret_cast<bool*>(chunk.data() + netvars::m_bIsScoped);
			dormant = *reinterpret_cast<bool*>(chunk.data() + offsets::m_bDormant);
			origin = *reinterpret_cast<vec3*>(chunk.data() + netvars::m_vecOrigin);
			view_offset = *reinterpret_cast<vec3*>(chunk.data() + netvars::m_vecViewOffset);
			eye_pos.x = view_offset.x += origin.x; eye_pos.y = view_offset.y += origin.y; eye_pos.z = view_offset.z += origin.z;
			velocity = *reinterpret_cast<vec3*>(chunk.data() + netvars::m_vecVelocity);
			punch = *reinterpret_cast<vec3*>(chunk.data() + netvars::m_vecPunch);
			fov = *reinterpret_cast<int*>(chunk.data() + netvars::m_iFOV);
		}

		ptr_t get_weapon() {
			ptr_t v;

			v = remote::read<ptr_t>(address + netvars::m_hActiveWeapon);
			return remote::read<ptr_t>(offsets::dwEntityList + ((v & 0xFFF) - 1) * 0x10);
		}

		void get_bone_matrix(int index, matrix3x4_t* out) {
			remote::read(remote::read<ptr_t>(address + netvars::m_dwBoneMatrix) + 0x30 * index, out, sizeof(matrix3x4_t));
		}

		bool is_valid() {
			return address && (health > 0) && !dormant;
		}
	};

	inline entity get_client_entity(int index) {
		return remote::read<entity>(offsets::dwEntityList + index * 0x10);
	}

	inline std::vector<entity> entities;
	inline std::shared_mutex entity_mutex;

	inline void update_entity_info(std::atomic<bool>& active) {
		while (active) {
			if (!engine::is_in_game()) {
				continue;
			}

			if (!entity_mutex.try_lock()) {
				continue;
			}

			int max_clients = engine::get_max_clients();

			entities.resize(max_clients);

			for (int i = 1; i <= max_clients; i++) {
				entities.at(i - 1) = get_client_entity(i);
			}

			entity_mutex.unlock();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}
