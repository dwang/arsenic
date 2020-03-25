#pragma once

#include "interfaces.hpp"
#include "types.hpp"
#include "../utilities/remote.hpp"

#include "../dependencies/lazy_importer/lazy_importer.hpp"
#include "../dependencies/xorstr/xorstr.hpp"

#include <string>

namespace cheat::sdk {
	struct convar {
		ptr_t address;

		SHORT_STRING get_name() {
			return remote::read_ptr32<SHORT_STRING>(address + 0xC);
		}

		SHORT_STRING get_string() {
			return remote::read_ptr32<SHORT_STRING>(address + 0x24);
		}

		void set_string(const char* v) {
			SHORT_STRING a0;
			LI_FN(memcpy)(&a0, v, strlen(v));
			remote::write_ptr<SHORT_STRING>(address + 0x24, a0);
		}

		int get_int() {
			return remote::read<int>(address + 0x30) ^ address;
		}

		void set_int(int v) {
			remote::write<std::uint32_t>(address + 0x30, v ^ address);
		}

		float get_float() {
			std::uint32_t v;
			v = remote::read<std::uint32_t>(address + 0x2C) ^ address;
			return *reinterpret_cast<float*>(&v);
		}

		void set_float(float v) {
			remote::write<std::uint32_t>(address + 0x2C, *reinterpret_cast<std::uint32_t*>(&v) ^ address);
		}
	};

	namespace cvar {
		inline convar sensitivity;
		inline convar mp_teammates_are_enemies;
		inline convar mp_weapons_glow_on_ground;

		inline convar find(const char* name) {
			ptr_t a0;

			a0 = remote::read_ptr32<ptr_t>(interfaces::cvar.address + 0x34);
			while (a0 = remote::read<ptr_t>(a0 + 0x4)) {
				if (!LI_FN(strcmp).safe_cached()(name, remote::read_ptr32<SHORT_STRING>(a0 + 0xC).value)) {
					return *reinterpret_cast<convar*>(&a0);
				}
			}

			throw name;
		}

		inline void initialize() {
			sensitivity = find(xorstr_("sensitivity"));
			mp_teammates_are_enemies = find(xorstr_("mp_teammates_are_enemies"));
			mp_weapons_glow_on_ground = find(xorstr_("mp_weapons_glow_on_ground"));

#ifdef _DEBUG
			printf(xorstr_(
				"  sensitivity:               0x%lx\n"
				"  mp_teammates_are_enemies:  0x%lx\n"
				"  mp_weapons_glow_on_ground: 0x%lx\n"
				"\n"),
				sensitivity.address,
				mp_teammates_are_enemies.address,
				mp_weapons_glow_on_ground.address
			);
#endif
		}
	}
}
