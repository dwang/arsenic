#pragma once

#include "types.hpp"
#include "../utilities/remote.hpp"

#include <cstdint>
#include <string>

namespace cheat::sdk {
	struct virtual_table {
		ptr_t address;

		ptr_t function(int index) {
			return remote::read<ptr_t>(remote::read<ptr_t>(address) + index * 4);
		}
	};
	
	struct interface_reg {
		ptr_t address;

		virtual_table find(const char* name) {
			ptr_t a0 = address, a1[30];

			do {
				remote::read(remote::read<ptr_t>(a0 + 0x4), &a1, sizeof(a1));
				if ((_stricmp(reinterpret_cast<const char*>(a1), name) >> 5) == 1) {
					return remote::read<virtual_table>(remote::read<ptr_t>(a0) + 1);
				}
			} while (a0 = remote::read<ptr_t>(a0 + 0x8));

			throw name;
		}
	};

	namespace interfaces {
		inline virtual_table client;
		inline virtual_table entity;
		inline virtual_table engine;
		inline virtual_table cvar;
		inline virtual_table input_system;

		inline interface_reg find(const wchar_t* name) {
			std::uintptr_t v;

			v = remote::find_export(remote::find_module(name), "CreateInterface");

			if (v == 0) {
				throw name;
			}

			return remote::read_ptr32<interface_reg>(v - 0x6A);
		}

		inline void initialize() {
			interface_reg t;

			t = interfaces::find(L"client_panorama.dll");
			client = t.find("VClient");
			entity = t.find("VClientEntityList");
			t = interfaces::find(L"engine.dll");
			engine = t.find("VEngineClient");
			t = interfaces::find(L"vstdlib.dll");
			cvar = t.find("VEngineCvar");
			t = interfaces::find(L"inputsystem.dll");
			input_system = t.find("InputSystemVersion");

#ifdef _DEBUG
			printf(
				"  VClient:                  0x%lx\n"
				"  VClientEntityList:        0x%lx\n"
				"  VEngineClient:            0x%lx\n"
				"  VEngineCvar:              0x%lx\n"
				"  InputSystemVersion:       0x%lx\n"
				"\n",
				client.address,
				entity.address,
				engine.address,
				cvar.address,
				input_system.address
			);
#endif
		}
	}
}
