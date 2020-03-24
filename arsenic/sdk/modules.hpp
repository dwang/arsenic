#pragma once

#include "../utilities/remote.hpp"

namespace cheat::sdk::modules {
	inline std::uintptr_t client;
	inline std::uintptr_t engine;
	
	inline void initialize() {
		client = remote::find_module(L"client_panorama.dll");
		engine = remote::find_module(L"engine.dll");

#ifdef _DEBUG
		printf(
			"  client_panorama.dll:      0x%lx\n"
			"  engine.dll:               0x%lx\n"
			"\n",
			client,
			engine
		);
#endif
	}
}
