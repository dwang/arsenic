#pragma once

#include "../sdk/entities.hpp"
#include "../sdk/types.hpp"

#include <atomic>

namespace cheat::features {
	extern void aim(std::atomic<bool>&);
	extern void aim_assist(sdk::entity, sdk::vec3, bool);
	extern void triggerbot(sdk::entity, sdk::vec3, bool);
}
