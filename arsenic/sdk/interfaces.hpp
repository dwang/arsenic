#pragma once

#include "types.hpp"
#include "../utilities/remote.hpp"

#include "../dependencies/lazy_importer/lazy_importer.hpp"
#include "../dependencies/xorstr/xorstr.hpp"

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
				if (LI_FN(strcmp).safe_cached()(reinterpret_cast<const char*>(a1), name) == 0) {
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

			v = remote::find_export(remote::find_module(name), xorstr_("CreateInterface"));

			if (v == 0) {
				throw name;
			}

			return remote::read_ptr32<interface_reg>(v - 0x6A);
		}

		inline void initialize() {
			interface_reg t;

			t = interfaces::find(xorstr_(L"client_panorama.dll"));
			client = t.find(xorstr_("VClient018"));
			entity = t.find(xorstr_("VClientEntityList003"));
			t = interfaces::find(xorstr_(L"engine.dll"));
			engine = t.find(xorstr_("VEngineClient014"));
			t = interfaces::find(xorstr_(L"vstdlib.dll"));
			cvar = t.find(xorstr_("VEngineCvar007"));
			t = interfaces::find(xorstr_(L"inputsystem.dll"));
			input_system = t.find(xorstr_("InputSystemVersion001"));

#ifdef _DEBUG
			printf(xorstr_(
				"  VClient018:               0x%lx\n"
				"  VClientEntityList003:     0x%lx\n"
				"  VEngineClient014:         0x%lx\n"
				"  VEngineCvar007:           0x%lx\n"
				"  InputSystemVersion001:    0x%lx\n"
				"\n"),
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
