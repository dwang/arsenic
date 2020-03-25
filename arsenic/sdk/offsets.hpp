#pragma once

#include "types.hpp"
#include "interfaces.hpp"
#include "../utilities/remote.hpp"

#include "../dependencies/xorstr/xorstr.hpp"

namespace cheat::sdk::offsets {
	inline ptr_t dwEntityList;
	inline ptr_t dwClientState;
	inline std::uint32_t m_dwGetLocalPlayer;
	inline std::uint32_t m_dwViewAngles;
	inline std::uint32_t m_dwMaxClients;
	inline std::uint32_t m_dwButton;
	inline std::uint32_t m_dwState;
	inline std::uint32_t m_dwAnalog;
	inline std::uint32_t m_dwAnalogDelta;
	inline std::uint32_t m_bDormant;

	inline void initialize() {
		dwEntityList = interfaces::entity.address - (remote::read<std::uint32_t>(interfaces::entity.function(5) + 0x22) - 0x38);
		dwClientState = remote::read_ptr32<ptr_t>(interfaces::engine.function(18) + 0x16);
		m_dwGetLocalPlayer = remote::read<std::uint32_t>(interfaces::engine.function(12) + 0x16);
		m_dwViewAngles = remote::read<std::uint32_t>(interfaces::engine.function(19) + 0xB2);
		m_dwMaxClients = remote::read<std::uint32_t>(interfaces::engine.function(20) + 0x07);
		m_dwState = remote::read<std::uint32_t>(interfaces::engine.function(26) + 0x07);
		m_dwButton = remote::read<std::uint32_t>(interfaces::input_system.function(15) + 0x21D);
		m_dwAnalog = remote::read<std::uint32_t>(interfaces::input_system.function(18) + 0x09);
		m_dwAnalogDelta = remote::read<std::uint32_t>(interfaces::input_system.function(18) + 0x29);
		m_bDormant = 0xED;

#ifdef _DEBUG
		printf(xorstr_(
			"  dwEntityList:              0x%lx\n"
			"  dwClientState:             0x%lx\n"
			"  m_dwGetLocalPlayer:        0x%x\n"
			"  m_dwViewAngles:            0x%x\n"
			"  m_dwMaxClients:            0x%x\n"
			"  m_dwState:                 0x%x\n"
			"  m_dwButton:                0x%x\n"
			"  m_dwAnalog:                0x%x\n"
			"  m_dwAnalogDelta:           0x%x\n"
			"  m_bDormant:                0x%x\n"
			"\n"),
			dwEntityList,
			dwClientState,
			m_dwGetLocalPlayer,
			m_dwViewAngles,
			m_dwMaxClients,
			m_dwState,
			m_dwButton,
			m_dwAnalog,
			m_dwAnalogDelta,
			m_bDormant
		);
#endif
	}
}
