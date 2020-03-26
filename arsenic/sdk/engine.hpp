#pragma once

#include "offsets.hpp"
#include "types.hpp"
#include "../utilities/remote.hpp"

namespace cheat::sdk::engine {
	inline bool is_running() {
		return remote::exists();
	}

	inline int get_local_player() {
		return remote::read<int>(offsets::dwClientState + offsets::m_dwGetLocalPlayer);
	}

	inline vec3 get_view_angles() {
		return remote::read<vec3>(offsets::dwClientState + offsets::m_dwViewAngles);
	}

	inline void set_view_angles(vec3 v) {
		remote::write<vec3>(offsets::dwClientState + offsets::m_dwViewAngles, v);
	}

	inline int get_max_clients() {
		return remote::read<int>(offsets::dwClientState + offsets::m_dwMaxClients);
	}

	inline bool is_in_game() {
		return remote::read<unsigned char>(offsets::dwClientState + offsets::m_dwState);
	}
}
