#pragma once

#include "interfaces.hpp"
#include "offsets.hpp"
#include "types.hpp"
#include "../utilities/remote.hpp"

namespace cheat::sdk::input_system {
	typedef enum _BUTTONCODE : int {
		MOUSE_1 = 107,
		MOUSE_2 = 108,
		MOUSE_3 = 109,
		MOUSE_4 = 110,
		MOUSE_5 = 111,
		END = 75
	} BUTTONCODE;

	inline bool is_button_down(BUTTONCODE button) {
		std::uint32_t v;

		v = remote::read<std::uint32_t>(interfaces::input_system.address + (((button >> 5) * 4) + offsets::m_dwButton));
		return (v >> (button & 31)) & 1;
	}
		
	inline vec2i get_mouse_analog() {
		return remote::read<vec2i>(interfaces::input_system.address + offsets::m_dwAnalog);
	}

	inline vec2i get_mouse_analog_delta() {
		return remote::read<vec2i>(interfaces::input_system.address + offsets::m_dwAnalogDelta);
	}
}
