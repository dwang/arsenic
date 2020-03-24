#pragma once

#define WIN32_LEAN_AND_MEAN

#include <thread>
#include <Windows.h>

#include "../sdk/interfaces.hpp"
#include "../sdk/modules.hpp"
#include "../utilities/remote.hpp"

namespace cheat::main {
	extern BOOL WINAPI detach();
	extern DWORD WINAPI attach(LPVOID);
}
