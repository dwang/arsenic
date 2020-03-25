#pragma once

#include "interfaces.hpp"
#include "types.hpp"
#include "../utilities/remote.hpp"

#include "../dependencies/lazy_importer/lazy_importer.hpp"
#include "../dependencies/xorstr/xorstr.hpp"

#include <string>

namespace cheat::sdk {
	struct netvar_table {
		ptr_t address;

		std::uint32_t offset(const char* name) {
			return offset_ex(address, name);
		}

		std::uint32_t offset_ex(ptr_t address, const char* name) {
            std::uint32_t a0, a1, a3, a5;
            ptr_t a2, a4;

            a0 = 0;

            for (a1 = remote::read<std::uint32_t>(address + 0x4); a1--;) {
                a2 = a1 * 60 + remote::read<ptr_t>(address);
                a3 = remote::read<std::uint32_t>(a2 + 0x2C);

                if ((a4 = remote::read<ptr_t>(a2 + 0x28)) && remote::read<std::uint32_t>(a4 + 0x4)) {
                    if (a5 = offset_ex(a4, name)) {
                        a0 += a3 + a5;
                    }
                }

                if (LI_FN(strcmp)(remote::read_ptr32<SHORT_STRING>(a2).value, name) == 0) {
                    return a3 + a0;
                }
            }

            return a0;
		}
	};

    namespace netvars {
        inline std::uint32_t m_iHealth;
        inline std::uint32_t m_vecViewOffset;
        inline std::uint32_t m_lifeState;
        inline std::uint32_t m_nTickBase;
        inline std::uint32_t m_vecVelocity;
        inline std::uint32_t m_vecPunch;
        inline std::uint32_t m_iFOV;
        inline std::uint32_t m_iTeamNum;
        inline std::uint32_t m_vecOrigin;
        inline std::uint32_t m_hActiveWeapon;
        inline std::uint32_t m_iShotsFired;
        inline std::uint32_t m_bIsScoped;
        inline std::uint32_t m_iGlowIndex;
        inline std::uint32_t m_dwBoneMatrix;
        
        inline netvar_table find(const char* name) {
            ptr_t a0, a1;

            a0 = remote::read_ptr32<ptr_t>(interfaces::client.function(8) + 1);

            do {
                a1 = remote::read<ptr_t>(a0 + 0xC);
                if (LI_FN(strcmp).safe_cached()(remote::read_ptr32<SHORT_STRING>(a1 + 0xC).value, name) == 0) {
                    return  *reinterpret_cast<netvar_table*>(&a1);
                }
            } while (a0 = remote::read<ptr_t>(a0 + 0x10));
        
            throw name;
        }

        inline void initialize() {
            netvar_table t;

            t = netvars::find(xorstr_("DT_BasePlayer"));
            m_iHealth = t.offset(xorstr_("m_iHealth"));
            m_vecViewOffset = t.offset(xorstr_("m_vecViewOffset[0]"));
            m_lifeState = t.offset(xorstr_("m_lifeState"));
            m_nTickBase = t.offset(xorstr_("m_nTickBase"));
            m_vecVelocity = t.offset(xorstr_("m_vecVelocity[0]"));
            m_vecPunch = t.offset(xorstr_("m_Local")) + 0x70;
            m_iFOV = t.offset(xorstr_("m_iFOV"));
            t = netvars::find(xorstr_("DT_BaseEntity"));
            m_iTeamNum = t.offset(xorstr_("m_iTeamNum"));
            m_vecOrigin = t.offset(xorstr_("m_vecOrigin"));
            t = netvars::find(xorstr_("DT_CSPlayer"));
            m_hActiveWeapon = t.offset(xorstr_("m_hActiveWeapon"));
            m_iShotsFired = t.offset(xorstr_("m_iShotsFired"));
            m_bIsScoped = t.offset(xorstr_("m_bIsScoped"));
            m_iGlowIndex = t.offset(xorstr_("m_flFlashDuration")) + 0x18;
            t = netvars::find(xorstr_("DT_BaseAnimating"));
            m_dwBoneMatrix = t.offset(xorstr_("m_nForceBone")) + 0x1C;

#ifdef _DEBUG
            printf(xorstr_(
                "  DT_BasePlayer:\n"
                "    m_iHealth:              0x%x\n"
                "    m_vecViewOffset:        0x%x\n"
                "    m_lifeState:            0x%x\n"
                "    m_nTickBase:            0x%x\n"
                "    m_vecVelocity:          0x%x\n"
                "    m_vecPunch:             0x%x\n"
                "    m_iFOV:                 0x%x\n"
                "  DT_BaseEntity:\n"
                "    m_iTeamNum:             0x%x\n"
                "    m_vecOrigin:            0x%x\n"
                "  DT_CSPlayer:\n"
                "    m_hActiveWeapon:        0x%x\n"
                "    m_iShotsFired:          0x%x\n"
                "    m_bIsScoped:            0x%x\n"
                "    m_iGlowIndex:           0x%x\n"
                "  DT_BaseAnimating:\n"
                "    m_dwBoneMatrix:         0x%x\n"
                "\n"),
                m_iHealth,
                m_vecViewOffset,
                m_lifeState,
                m_nTickBase,
                m_vecVelocity,
                m_vecPunch,
                m_iFOV,
                m_iTeamNum,
                m_vecOrigin,
                m_hActiveWeapon,
                m_iShotsFired,
                m_bIsScoped,
                m_iGlowIndex,
                m_dwBoneMatrix
            );
#endif
        }
    }
}
