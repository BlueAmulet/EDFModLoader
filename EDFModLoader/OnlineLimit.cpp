#include <windows.h>
#include <cstdint>

#include "OnlineLimit.h"

// Injects hook into game process
void __fastcall WriteHookToProcess(void *addr, void *data, size_t len) {
	DWORD oldProtect;
	VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(addr, data, len);
	VirtualProtect(addr, len, oldProtect, &oldProtect);
	// Refresh cpu instruction?
	// FlushInstructionCache(handleEXE, addr, len);
}

// update game's original functions with interruption, need 15 bytes
void __fastcall hookGameBlockWithInt3(void *targetAddr, uintptr_t dataAddr) {
	uint8_t hookFunction[] = {
	    0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,             // jmp
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // addr
	    0xCC                                            // int3
	};
	memcpy(&hookFunction[6], &dataAddr, sizeof(dataAddr));

	WriteHookToProcess(targetAddr, hookFunction, sizeof(hookFunction));
}

// Blocking EDF5 creates public rooms wherever possible.
void __fastcall EDF5OnlineRoomLimit(PBYTE hmodEXE) {
	// forced private room creation
	// everyone, offset is 0x57F88F
	unsigned char everyone1[] = {0x82, 0x94};
	WriteHookToProcess((void *)(hmodEXE + 0x58048F + 3), &everyone1, 2U);
	unsigned char everyone2[] = {0x3C, 0x78};
	WriteHookToProcess((void *)(hmodEXE + 0x5820D5 + 3), &everyone2, 2U);
	// need password, offset is 0x57FAD0
	unsigned char password1[] = {0x41};
	WriteHookToProcess((void *)(hmodEXE + 0x5806D0 + 3), &password1, 1U);
	unsigned char password2[] = {0xDC, 0x78};
	WriteHookToProcess((void *)(hmodEXE + 0x582035 + 3), &password2, 2U);
}

extern "C" {
void __fastcall ASMreadMissionSavaData();
uintptr_t readMissionSavaDataRetAddr;
}

// Disable online when mod folder is enabled.
void __fastcall EDF6OnlineRoomLimit(PBYTE hmodEXE, uintptr_t OlOfs) {
	// edf.dll+E2970, Determine mission savedata location, R8 is 1 when it is online.
	hookGameBlockWithInt3((void *)(hmodEXE + OlOfs), (uintptr_t)ASMreadMissionSavaData);
	readMissionSavaDataRetAddr = (uintptr_t)(hmodEXE + OlOfs+0x10);
}
