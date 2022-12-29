#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#define PLOG_OMIT_LOG_DEFINES
//#define PLOG_EXPORT

#include <vector>
#include <cstdio>
#include <memory.h>

#include <windows.h>
#include <shlwapi.h>

#include <HookLib.h>
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

#include "proxy.h"
#include "PluginAPI.h"
#include "LoggerTweaks.h"


typedef struct {
	PluginInfo *info;
	void *module;
} PluginData;

static std::vector<PluginData*> plugins; // Holds all plugins loaded
typedef bool (__fastcall *LoadDef)(PluginInfo*);

static std::vector<void*> hooks; // Holds all original hooked functions

// Called during initialization of CRT
typedef void* (__fastcall *initterm_func)(void*, void*);
static initterm_func initterm_orig;
// Handles opening files from disk or through CRI File System
typedef void* (__fastcall *fnk244d0_func)(void*, void*, void*);
static fnk244d0_func fnk244d0_orig;
// Puts wide string in weird string structure
typedef void* (__fastcall *fnk27380_func)(void*, const wchar_t*, unsigned long long);
static fnk27380_func fnk27380_orig;
// printf-like logging stub, usually given wide strings, sometimes normal strings
typedef void (__fastcall *gamelog_func)(const wchar_t*);
static gamelog_func gamelog_orig;

// Verify PluginData->module can store a HMODULE
static_assert(sizeof(HMODULE) == sizeof(PluginData::module), "module field cannot store an HMODULE");

// Minor utility functions
static inline BOOL FileExistsW(LPCWSTR szPath) {
	DWORD dwAttrib = GetFileAttributesW(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

static BOOL GetPrivateProfileBoolW(LPCWSTR lpAppName, LPCWSTR lpKeyName, BOOL bDefault, LPCWSTR lpFileName) {
	WCHAR boolStr[6];
	DWORD strlen = GetPrivateProfileStringW(lpAppName, lpKeyName, bDefault ? L"True" : L"False", boolStr, _countof(boolStr), lpFileName);
	return (CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, boolStr, strlen, L"True", 4) == CSTR_EQUAL);
}

template <size_t N>
constexpr size_t cwslen(wchar_t const (&)[N]) {
	return N - 1;
}

#define wcsstart(a, b) (!wcsncmp(a, b, cwslen(b)))

// Hook wrapper functions
BOOLEAN EDFMLAPI SetHookWrap(const void *Interceptor, void **Original) {
	if (Original != NULL && *Original != NULL && SetHook(*Original, Interceptor, Original)) {
		hooks.push_back(*Original);
		return true;
	} else {
		return false;
	}
}

BOOLEAN EDFMLAPI RemoveHookWrap(void *Original) {
	if (Original != NULL) {
		std::vector<void*>::iterator position = std::find(hooks.begin(), hooks.end(), Original);
		if (position != hooks.end()) {
			if (RemoveHook(Original)) {
				hooks.erase(position);
				return true;
			}
		}
	}
	return false;
}

static void RemoveAllHooks(void) {
	for (std::vector<void*>::iterator it = hooks.begin(); it != hooks.end();) {
		void *hook = *it;
		if (RemoveHook(hook)) {
			it = hooks.erase(it);
		} else {
			// hook is HOOK_DATA->OriginalBeginning
			// hook-16 is HOOK_DATA->OriginalFunction
			// TODO: Fork HookLib and expose HOOK_DATA or add function to retrieve original address
			PVOID address = *((PVOID*)hook - 16 / sizeof(PVOID));

			HMODULE hmodDLL;
			wchar_t DLLName[MAX_PATH];
			GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)address, &hmodDLL);
			GetModuleFileNameW(hmodDLL, DLLName, _countof(DLLName));
			PathStripPathW(DLLName);

			PLOG_ERROR << "Failed to remove " << DLLName << "+" << std::hex << ((ULONG_PTR)address - (ULONG_PTR)hmodDLL) << " hook";
			it++;
		}
	}
}

// Configuration
static BOOL LoadPluginsB = TRUE;
static BOOL Redirect = TRUE;
static BOOL GameLog = FALSE;

// Pointer sets
typedef struct {
	uintptr_t offset;
	const wchar_t *search;
	const char *ident;
	const char *plugfunc;
	uintptr_t pointers[4];
} PointerSet;

int pointerSet = -1;
PointerSet psets[2] = { //
	{0xebcbd0, L"EarthDefenceForce 5 for PC", "EDF5", "EML5_Load", {0x9c835a, 0x244d0, 0x27380, 0x27680}}, // EDF 5
	{0xaa36d0, L"EarthDefenceForce 4.1 for Windows", "EDF41", "EML4_Load", {0x667102, 0x8ed80, 0x91580, 0x91790}}, // EDF 4.1
};

// Search and load all *.dll files in Mods\Plugins\ folder
static void LoadPlugins(void) {
	WIN32_FIND_DATAW ffd;
	PLOG_INFO << "Loading plugins";
	HANDLE hFind = FindFirstFileW(L"Mods\\Plugins\\*.dll", &ffd);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				PLOG_INFO << "Loading Plugin: " << ffd.cFileName;
				wchar_t plugpath[MAX_PATH];
				wcscpy_s(plugpath, L"Mods\\Plugins\\");
				wcscat_s(plugpath, ffd.cFileName);
				HMODULE plugin = LoadLibraryW(plugpath);
				if (plugin != NULL) {
					LoadDef loadfunc = (LoadDef)GetProcAddress(plugin, psets[pointerSet].plugfunc);
					bool unload = false;
					if (loadfunc != NULL) {
						PluginInfo *pluginInfo = new PluginInfo();
						pluginInfo->infoVersion = 0;
						if (loadfunc(pluginInfo)) {
							// Validate PluginInfo
							if (pluginInfo->infoVersion == 0) {
								PLOG_ERROR << "PluginInfo infoVersion 0, expected " << PluginInfo::MaxInfoVer;
								unload = true;
							} else if (pluginInfo->name == NULL) {
								PLOG_ERROR << "Plugin missing name";
								unload = true;
							} else if (pluginInfo->infoVersion > PluginInfo::MaxInfoVer) {
								PLOG_ERROR << "Plugin has unsupported infoVersion " << pluginInfo->infoVersion << " expected " << PluginInfo::MaxInfoVer;
								unload = true;
							} else {
								switch (pluginInfo->infoVersion) {
								case 1:
								default:
									// Latest info version
									PluginData *pluginData = new PluginData;
									pluginData->info = pluginInfo;
									pluginData->module = plugin;
									plugins.push_back(pluginData);
									break;
								}
								static_assert(PluginInfo::MaxInfoVer == 1, "Supported version changed, update version handling and this number");
							}
						} else {
							PLOG_INFO << "Unloading plugin";
							unload = true;
						}
						if (unload) {
							delete pluginInfo;
						}
					} else {
						PLOG_WARNING << "Plugin does not contain " << psets[pointerSet].plugfunc << " function";
						unload = true;
					}
					if (unload) {
						FreeLibrary(plugin);
					}
				} else {
					DWORD dwError = GetLastError();
					PLOG_ERROR << "Failed to load plugin: error " << dwError;
				}
			}
		} while (FindNextFileW(hFind, &ffd) != 0);
		// Check if finished with error
		DWORD dwError = GetLastError();
		if (dwError != ERROR_NO_MORE_FILES) {
			PLOG_ERROR << "Failed to search for plugins: error " << dwError;
		}
		FindClose(hFind);
	} else {
		DWORD dwError = GetLastError();
		if (dwError != ERROR_FILE_NOT_FOUND && dwError != ERROR_PATH_NOT_FOUND) {
			PLOG_ERROR << "Failed to search for plugins: error " << dwError;
		}
	}
}

// Early hook into game process
static void* __fastcall initterm_hook(void *unk1, void *unk2) {
	static bool initialized = false;
	if (!initialized) {
		initialized = true;
		PLOG_INFO << "Additional initialization";

		// Load plugins
		if (LoadPluginsB) {
			LoadPlugins();
		} else {
			PLOG_INFO << "Plugin loading disabled";
		}

		PLOG_INFO << "Initialization finished";
	}
	return initterm_orig(unk1, unk2);
}

struct oddstr {
	wchar_t *str;
	void *unk;
	size_t unk2;
	size_t length;
};
static_assert(sizeof(oddstr) == 32, "Weird string structure should have a length of 32");

static void *__fastcall fnk244d0_hook(void *unk1, oddstr *str, void *unk2) {
	// First 8 bytes are a pointer if length >= 8
	// Otherwise string is stored where pointer will be?
	wchar_t *path = (wchar_t*)str;
	if (str->length >= 8) {
		path = str->str;
	}
	if (path != NULL && str->length >= cwslen(L"/cri_bind/") && wcsstart(path, L"/cri_bind/")) {
		size_t newlen = str->length + cwslen(L"./Mods/") - cwslen(L"/cri_bind/");
		wchar_t *modpath = new wchar_t[newlen + 1];
		wcscpy(modpath, L"./Mods/");
		wmemcpy(modpath + cwslen(L"./Mods/"), path + cwslen(L"/cri_bind/"), str->length - cwslen(L"/cri_bind/"));
		modpath[newlen] = L'\0';
		PLOG_DEBUG << "Checking for " << modpath;
		if (FileExistsW(modpath)) {
			PLOG_DEBUG << "Redirecting access to " << modpath;
			fnk27380_orig(str, modpath, newlen);
		}
		delete[] modpath;
	}
	return fnk244d0_orig(unk1, str, unk2);
}

// Internal logging hook
extern "C" {
void __fastcall gamelog_hook(const wchar_t *fmt, ...);   // wrapper to preserve registers
void __fastcall gamelog_hook_main(const char *fmt, ...); // actual logging implementaton

// Thread Local Storage to preserve/restore registers in wrapper
// TODO: Move all of this to winmm.asm
__declspec(thread) UINT64 save_ret;
__declspec(thread) UINT64 save_rax;
__declspec(thread) UINT64 save_rcx;
__declspec(thread) UINT64 save_rdx;
__declspec(thread) UINT64 save_r8;
__declspec(thread) UINT64 save_r9;
__declspec(thread) UINT64 save_r10;
__declspec(thread) UINT64 save_r11;

__declspec(thread) M128A save_xmm0;
__declspec(thread) M128A save_xmm1;
__declspec(thread) M128A save_xmm2;
__declspec(thread) M128A save_xmm3;
__declspec(thread) M128A save_xmm4;
__declspec(thread) M128A save_xmm5;
}

void __fastcall gamelog_hook_main(const char *fmt, ...) {
	if (fmt != NULL) {
		va_list args;
		va_start(args, fmt);
		if (fmt[0] == 'L' && fmt[1] == '\0' && !wcscmp((wchar_t*)fmt, L"LoadComplete:%s %s %d\n")) {
			// This wide string is formatted with normal strings
			fmt = "LoadComplete:%s %s %d";
		}
		// This is sometimes called with wide strings and normal strings
		// Try to automatically detect
		if (fmt[0] != '\0' && fmt[1] == '\0') {
			int required = _vsnwprintf(NULL, 0, (wchar_t*)fmt, args);
			wchar_t *buffer = new wchar_t[(size_t)required + 1];
			_vsnwprintf(buffer, (size_t)required + 1, (wchar_t*)fmt, args);
			va_end(args);
			// Remove new line from end of message if present
			if (required >= 1 && buffer[required - 1] == L'\n') {
				buffer[required - 1] = L'\0';
			}
			PLOG_INFO_(1) << buffer;
			delete[] buffer;
		} else {
			int required = _vsnprintf(NULL, 0, fmt, args);
			char *buffer = new char[(size_t)required + 1];
			_vsnprintf(buffer, (size_t)required + 1, fmt, args);
			va_end(args);
			// See above comment
			if (required >= 1 && buffer[required - 1] == '\n') {
				buffer[required - 1] = '\0';
			}
			PLOG_INFO_(1) << buffer;
			delete[] buffer;
		}
	} else {
		PLOG_INFO_(1) << "(null)";
	}
}

// Names for the log formatter
static const char ModLoaderStr[] = "ModLoader";

PBYTE hmodEXE;
char hmodName[MAX_PATH];

void SetupHook(uintptr_t offset, void **func, void* hook, const char *reason, BOOL active) {
	if (active) {
		PLOG_INFO << "Hooking " << hmodName << "+" << std::hex << offset << " (" << reason << ")";
		*func = hmodEXE + offset;
		if (!SetHookWrap(hook, func)) {
			// Error
			PLOG_ERROR << "Failed to setup " << hmodName << "+" << std::hex << offset << " hook";
		}
	} else {
		PLOG_INFO << "Skipping " << hmodName << "+" << std::hex << offset << " hook (" << reason << ")";
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	static plog::RollingFileAppender<eml::TxtFormatter<ModLoaderStr>> mlLogOutput("ModLoader.log");
	static plog::RollingFileAppender<eml::TxtFormatter<nullptr>> gameLogOutput("game.log");

	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		// For optimization
		DisableThreadLibraryCalls(hModule);

		// Calculate ini path
		WCHAR iniPath[MAX_PATH];
		GetModuleFileNameW(hModule, iniPath, _countof(iniPath));
		PathRemoveFileSpecW(iniPath);
		wcscat_s(iniPath, L"\\ModLoader.ini");

		// Read configuration
		LoadPluginsB = GetPrivateProfileBoolW(L"ModLoader", L"LoadPlugins", LoadPluginsB, iniPath);
		Redirect = GetPrivateProfileBoolW(L"ModLoader", L"Redirect", Redirect, iniPath);
		GameLog = GetPrivateProfileBoolW(L"ModLoader", L"GameLog", GameLog, iniPath);

		// Open Log file
		DeleteFileW(L"ModLoader.log");
#ifdef NDEBUG
		plog::init(plog::info, &mlLogOutput);
#else
		plog::init(plog::debug, &mlLogOutput);
#endif
		if (GameLog) {
			DeleteFileW(L"game.log");
			plog::init<1>(plog::info, &gameLogOutput);
		}

		// Add ourself to plugin list for future reference
		PluginInfo *selfInfo = new PluginInfo;
		selfInfo->infoVersion = PluginInfo::MaxInfoVer;
		selfInfo->name = "EDFModLoader";
		selfInfo->version = PLUG_VER(1, 0, 7, 1);
		PluginData *selfData = new PluginData;
		selfData->info = selfInfo;
		selfData->module = hModule;
		plugins.push_back(selfData);

		// Determine what game is hosting us
		hmodEXE = (PBYTE)GetModuleHandleW(NULL);
		GetModuleFileNameA((HMODULE)hmodEXE, hmodName, _countof(hmodName));
		char *hmodFName = PathFindFileNameA(hmodName);
		memmove(hmodName, hmodFName, strlen(hmodFName) + 1);
		for (int i = 0; i < _countof(psets); i++) {
			size_t search_len = wcslen(psets[i].search);
			if (!IsBadReadPtr(hmodEXE + psets[i].offset, search_len+1) && !wcsncmp((wchar_t*)(hmodEXE + psets[i].offset), psets[i].search, search_len)) {
				pointerSet = i;
				break;
			}
		}
		if (pointerSet == -1) {
			PLOG_ERROR << "Failed to determine what exe is running";
			return FALSE;
		}
		uintptr_t *pointers = psets[pointerSet].pointers;

		PluginVersion v = selfInfo->version;
		PLOG_INFO.printf("EDFModLoader (%s) v%u.%u.%u Initializing\n", psets[pointerSet].ident, v.major, v.minor, v.patch);

		// Setup DLL proxy
		wchar_t path[MAX_PATH];
		if (!GetWindowsDirectoryW(path, _countof(path))) {
			DWORD dwError = GetLastError();
			PLOG_ERROR << "Failed to get windows directory path: error " << dwError;
			return FALSE;
		}

		wcscat_s(path, L"\\System32\\winmm.dll");

		PLOG_INFO << "Loading real winmm.dll";
		PLOG_INFO << "Setting up dll proxy functions";
		setupFunctions(LoadLibraryW(path));

		// Create ModLoader folders
		CreateDirectoryW(L"Mods", NULL);
		CreateDirectoryW(L"Mods\\Plugins", NULL);

		// Hook function for additional ModLoader initialization
		SetupHook(pointers[0], (PVOID*)&initterm_orig, initterm_hook, "Additional initialization", TRUE);

		// Add Mods folder redirector hook
		fnk27380_orig = (fnk27380_func)((PBYTE)hmodEXE + pointers[2]);
		SetupHook(pointers[1], (PVOID*)&fnk244d0_orig, fnk244d0_hook, "Mods folder redirector", Redirect);

		// Add internal logging hook
		SetupHook(pointers[3], (PVOID*)&gamelog_orig, gamelog_hook, "Interal logging hook", GameLog);

		// Finished
		PLOG_INFO << "Basic initialization complete";

		break;
	}
	case DLL_PROCESS_DETACH: {
		PLOG_INFO << "EDFModLoader Unloading";

		// Remove hooks
		PLOG_INFO << "Removing hooks";
		RemoveAllHooks();

		// Unload all plugins
		PLOG_INFO << "Unloading plugins";
		for (PluginData *pluginData : plugins) {
			delete pluginData->info;
			if (pluginData->module != hModule) {
				FreeLibrary((HMODULE)(pluginData->module));
			}
			delete pluginData;
		}
		plugins.clear();

		// Unload real winmm.dll
		PLOG_INFO << "Unloading real winmm.dll";
		cleanupProxy();

		// TODO: Close log file?
		break;
	}
	}
	return TRUE;
}
