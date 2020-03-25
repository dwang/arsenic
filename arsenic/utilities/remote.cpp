#include "remote.hpp"

#include "../dependencies/xorstr/xorstr.hpp"

#include <string>

#pragma comment(lib, "ntdll.lib")

#ifdef _WIN64
#define SNAP_LENGTH 0x188
#define SNAP_NAME   0x040
#define TEB_WOW64   0x100
#define SNAP_PID    0x128
#define SNAP_START  0x160
#define SNAP_TEB    0x168
#else
#define SNAP_LENGTH 0x110
#define SNAP_NAME   0x03C
#define TEB_WOW64   0x0C0
#define SNAP_PID    0x0D8
#define SNAP_START  0x100
#define SNAP_TEB    0x104
#endif

#define IS_WOW64_ADDRESS(address) (address <= 0xffffffffUL)

extern "C" {
	typedef void* voidptr_t;
	typedef long NTSTATUS;
#define NTAPI __stdcall
#define WINAPI __stdcall
#define DECLSPEC_IMPORT __declspec(dllimport)
	DECLSPEC_IMPORT NTSTATUS NTAPI NtQuerySystemInformation(std::uint32_t, voidptr_t, std::uint32_t, std::uint32_t*);
	DECLSPEC_IMPORT NTSTATUS NTAPI NtClose(HANDLE);
	DECLSPEC_IMPORT NTSTATUS NTAPI NtReadVirtualMemory(HANDLE, PVOID, PVOID, std::size_t, std::size_t*);
	DECLSPEC_IMPORT NTSTATUS NTAPI NtWriteVirtualMemory(HANDLE, PVOID, PVOID, std::size_t, std::size_t*);
	DECLSPEC_IMPORT HANDLE WINAPI OpenProcess(std::uint32_t, std::uint32_t, std::uint32_t);
}

struct process_entry {
	const wchar_t* name;
	int pid;
	std::uintptr_t start;
	std::uintptr_t teb;
};

struct process_snap {
	char* first, *entry;
};

void* create_snapshot() {
	std::uint32_t length = 0;
	NTSTATUS status;

	status = NtQuerySystemInformation(57, &length, SNAP_LENGTH, &length);

	if (status != 0xC0000004L) {
		return 0;
	}

	struct process_snap* snap = new process_snap;

	length += 4096;
	snap->first = new char[length];
	status = NtQuerySystemInformation(57, snap->first, length, &length);

	if (status != 0L) {
		delete[] snap->first;
		delete snap;
		return 0;
	}
	else {
		snap->entry = snap->first;
	}

	return snap;
}

void close_snapshot(void* snapshot) {
	struct process_snap* snap = reinterpret_cast<struct process_snap*>(snapshot);

	delete[] snap->first;
	delete[] snap;
}

bool get_next_process(void* snapshot, struct process_entry* entry) {
	struct process_snap* snap = reinterpret_cast<struct process_snap*>(snapshot);

	if (*reinterpret_cast<uint32_t*>(snap->entry) == 0) {
		return false;
	}

	snap->entry = snap->entry + *reinterpret_cast<uint32_t*>(snap->entry);
	entry->name = *reinterpret_cast<const wchar_t**>(snap->entry + SNAP_NAME);
	entry->pid = *reinterpret_cast<int*>(snap->entry + SNAP_PID);
	entry->start = *reinterpret_cast<std::uintptr_t*>(snap->entry + SNAP_START);
	entry->teb = *reinterpret_cast<std::uintptr_t*>(snap->entry + SNAP_TEB);

	return true;
}

std::uintptr_t teb_to_peb(HANDLE process, std::uintptr_t teb, bool wow64) {
	std::uintptr_t peb = 0;
	int length, offset;

	if (wow64) {
		length = 4, offset = 0x2030;
	}
	else {
		length = 8, offset = 0x60;
	}

	NtReadVirtualMemory(process, reinterpret_cast<PVOID>(reinterpret_cast<char*>(teb) + offset), &peb, length, 0);

	return peb;
}

namespace cheat::remote {
	static HANDLE handle;
	static bool wow64;
	static std::uintptr_t peb;

	bool attach(const wchar_t* name) {
		void* snap = create_snapshot();

		if (snap == 0) {
			return false;
		}

		struct process_entry entry;

		while (get_next_process(snap, &entry)) {
			if (!_wcsicmp(entry.name, name)) {
				handle = OpenProcess(0x1fffff, 0, entry.pid);
				wow64 = IS_WOW64_ADDRESS(entry.start);
				peb = teb_to_peb(handle, entry.teb, wow64);
#ifdef _DEBUG
				printf(xorstr_("  %ls:                 %d\n\n"), name, entry.pid);
#endif
				break;
			}
		}

		close_snapshot(snap);
		return handle != 0;
	}

	void detach() {
		if (handle != 0) {
			NtClose(handle);
		}
	}

	bool exists() {
		char b;
		return read(peb, &b, 1) == 0l;
	}

	std::uintptr_t find_module(const wchar_t* name) {
		std::uintptr_t a0 = 0, a1 = 0, a2 = 0, a3[30];
		std::uint32_t rly[5];

		a3[0] = '\0';

		if (wow64) {
			rly[0] = 0x04, rly[1] = 0x0C, rly[2] = 0x14, rly[3] = 0x28, rly[4] = 0x10;
		}
		else {
			rly[0] = 0x08, rly[1] = 0x18, rly[2] = 0x20, rly[3] = 0x50, rly[4] = 0x20;
		}

		read(peb + rly[1], &a0, rly[0]);
		read(a0 + rly[2], &a0, rly[0]);
		read(a0 + rly[0], &a1, rly[0]);

		while (a0 != a1) {
			read(a0 + rly[3], &a2, rly[0]);
			read(a2, &a3, sizeof(a3));

			if (_wcsicmp(reinterpret_cast<const wchar_t*>(a3), name) == 0) {
				read(a0 + rly[4], &a0, rly[0]);
				return a0;
			}

			if (read(a0, &a0, rly[0]) != 0l) {
				break;
			}
		}

		return 0;
	}

	std::uintptr_t find_export(std::uintptr_t module, const char* name) {
		std::uintptr_t a0;
		std::uint32_t a1[4], a2[30];

		a2[0] = '\0';

		a0 = module + read<std::uint16_t>(module + 0x3C);
		a0 = module + read<std::uint32_t>(a0 + 0x88 - wow64 * 16);
		read(a0 + 0x18, &a1, sizeof(a1));

		while (a1[0]--) {
			a0 = read<std::uint32_t>(module + a1[2] + (a1[0] * 4));
			read(module + a0, &a2, sizeof(a2));

			if (!_stricmp(reinterpret_cast<const char*>(a2), name)) {
				return (module + read<std::uint32_t>(module + a1[1] + (read<std::uint16_t>(module + a1[3] + (a1[0] * 2)) * 4)));
			}
		}

		return 0;
	}

	NTSTATUS read(std::uintptr_t address, void* buffer, std::size_t length) {
		return NtReadVirtualMemory(handle, reinterpret_cast<PVOID>(address), buffer, length, 0);
	}

	NTSTATUS write(std::uintptr_t address, void* buffer, std::size_t length) {
		return NtWriteVirtualMemory(handle, reinterpret_cast<PVOID>(address), buffer, length, 0);
	}
}
