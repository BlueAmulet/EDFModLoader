#pragma once

// Injects hook into game process
void __fastcall WriteHookToProcess(void *addr, void *data, size_t len);
// update game's original functions with interruption, need 15 bytes
void __fastcall hookGameBlockWithInt3(void *targetAddr, uintptr_t dataAddr);

// Blocking EDF5 creates public rooms wherever possible.
void __fastcall EDF5OnlineRoomLimit(PBYTE hmodEXE);

// Disable online when mod folder is enabled.
void __fastcall EDF6OnlineRoomLimit(PBYTE hmodEXE, uintptr_t OlOfs);
