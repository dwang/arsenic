#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

namespace cheat::main {
	extern BOOL WINAPI detach();
	extern DWORD WINAPI attach(LPVOID);
}
