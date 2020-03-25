#pragma once

#define WIN32_LEAN_AND_MEAN

#include "../sdk/convars.hpp"
#include "../sdk/interfaces.hpp"
#include "../sdk/modules.hpp"
#include "../sdk/netvars.hpp"
#include "../sdk/offsets.hpp"
#include "../utilities/remote.hpp"

#include "../dependencies/lazy_importer/lazy_importer.hpp"
#include "../dependencies/xorstr/xorstr.hpp"

#include <thread>
#include <Windows.h>

namespace cheat::main {
	extern BOOL WINAPI detach();
	extern DWORD WINAPI attach(LPVOID);
}
