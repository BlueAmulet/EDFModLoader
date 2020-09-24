#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#define PLOG_OMIT_LOG_DEFINES
//#define PLOG_EXPORT

#include <vector>
#include <cstdio>

#include <Windows.h>
#include <shlwapi.h>
#include <HookLib.h>
#include <memory.h>

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

// Called one at beginning and end of game, hooked to perform additional initialization
typedef int (__fastcall *fnk3d8f00_func)(char);
static fnk3d8f00_func fnk3d8f00_orig;
// Called for various file paths (and more), hooked to redirect file access to Mods folder
typedef void* (__fastcall *fnk27380_func)(void*, const wchar_t*, unsigned long long);
static fnk27380_func fnk27380_orig;
// Called for built file paths (and more), hooked to redirect file access to Mods folder
typedef void* (__fastcall *fnk3e420_func)(void*, const wchar_t*, unsigned long long);
static fnk3e420_func fnk3e420_orig;
// printf-like logging stub, usually given wide strings, sometimes normal strings
typedef void (__fastcall *fnk27680_func)(const wchar_t*);
static fnk27680_func fnk27680_orig;

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
			// TODO: Fork HookLib and exposde HOOK_DATA or add function to retrieve original address
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
					LoadDef loadfunc = (LoadDef)GetProcAddress(plugin, "EML5_Load");
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
						PLOG_WARNING << "Plugin does not contain EML5_Load function";
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

// Add "+ModLoader" to game window
static void ModifyTitle(void) {
	HWND edfHWND = FindWindowW(L"xgs::Framework", L"EarthDefenceForce 5 for PC");
	if (edfHWND != NULL) {
		int length = GetWindowTextLengthW(edfHWND);
		const wchar_t *suffix = L" +ModLoader";
		wchar_t *buffer = new wchar_t[length + wcslen(suffix) + 1];
		GetWindowTextW(edfHWND, buffer, length + wcslen(suffix) + 1);
		wcscat(buffer, suffix);
		SetWindowTextW(edfHWND, buffer);
		delete[] buffer;
	} else {
		PLOG_WARNING << "Failed to get window handle to EDF5";
	}
}

// Early hook into game process
static int __fastcall fnk3d8f00_hook(char unk) {
	// Called with 0 at beginning, 1 later
	if (unk == 0) {
		PLOG_INFO << "Additional initialization";

		// Load plugins
		if (LoadPluginsB) {
			LoadPlugins();
		} else {
			PLOG_INFO << "Plugin loading disabled";
		}

		PLOG_INFO << "Initialization finished";
	}
	return fnk3d8f00_orig(unk);
}

struct oddstr {
	wchar_t *str;
	void *unk;
	size_t unk2;
	size_t length;
};
static_assert(sizeof(oddstr) == 32, "Weird string structure should have a length of 32");

// app:/ to Mods folder redirector
static void *__fastcall fnk27380_hook(oddstr *str, const wchar_t *path, size_t length) {
	// path is not always null terminated
	// length does not include null terminator
	if (path != NULL && length > 0) {
		IF_PLOG(plog::verbose) {
			wchar_t *npath = new wchar_t[length + 1];
			wmemcpy(npath, path, length);
			npath[length] = L'\0';
			PLOG_VERBOSE << "Hook1: " << npath;
			delete[] npath;
		}
	}
	if (path != NULL && length >= cwslen(L"app:/") && _wcsnicmp(L"app:/", path, cwslen(L"app:/")) == 0 && wmemchr(path, L'.', length)) {
		size_t newlen = length + cwslen(L"./Mods/") - cwslen(L"app:/");
		wchar_t *modpath = new wchar_t[newlen + 1];
		wcscpy(modpath, L"./Mods/");
		wmemcpy(modpath + wcslen(modpath), path + cwslen(L"app:/"), length - cwslen(L"app:/"));
		modpath[newlen] = L'\0';
		PLOG_DEBUG << "Checking for " << modpath;
		if (FileExistsW(modpath)) {
			PLOG_DEBUG << "Redirecting access to " << modpath;
			void *ret = fnk27380_orig(str, modpath, newlen);
			delete[] modpath;
			return ret;
		} else {
			delete[] modpath;
		}
	}

	void *ret = fnk27380_orig(str, path, length);
	return ret;
}

// app:/ to Mods folder redirector part 2
static void *__fastcall fnk3e420_hook(oddstr *str, const wchar_t *part, size_t length) {
	void *ret = fnk3e420_orig(str, part, length);
	// First 8 bytes are a pointer if length >= 8
	// Otherwise string is stored where pointer will be?
	wchar_t *path = (wchar_t*)str;
	if (str->length >= 8) {
		path = str->str;
	}
	if (path != NULL && str->length > 0) {
		IF_PLOG(plog::verbose) {
			wchar_t *npath = new wchar_t[str->length + 1];
			wmemcpy(npath, path, str->length);
			npath[str->length] = L'\0';
			PLOG_VERBOSE << "Hook2: " << npath;
			delete[] npath;
		}
	}
	if (path != NULL && str->length >= cwslen(L"app:/") && _wcsnicmp(L"app:/", path, cwslen(L"app:/")) == 0 && wmemchr(path, L'.', str->length)) {
		size_t newlen = str->length + cwslen(L"./Mods/") - cwslen(L"app:/");
		wchar_t *modpath = new wchar_t[newlen + 1];
		wcscpy(modpath, L"./Mods/");
		wmemcpy(modpath + cwslen(L"./Mods/"), path + cwslen(L"app:/"), str->length - cwslen(L"app:/"));
		modpath[newlen] = L'\0';
		PLOG_DEBUG << "Checking for " << modpath;
		if (FileExistsW(modpath)) {
			PLOG_DEBUG << "Redirecting access to " << modpath;
			fnk27380_orig(str, modpath, newlen); // fnk27380 used intentionally
		}
		delete[] modpath;
	}
	return ret;
}

// Internal logging hook
extern "C" {
void __fastcall fnk27680_hook(const wchar_t *fmt, ...);   // wrapper to preserve registers
void __fastcall fnk27680_hook_main(const char *fmt, ...); // actual logging implementaton

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

void __fastcall fnk27680_hook_main(const char *fmt, ...) {
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
static const char GameStr[] = "Game";

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	static plog::RollingFileAppender<eml::TxtFormatter<ModLoaderStr>> mlLogOutput("ModLoader.log");
	static plog::RollingFileAppender<eml::TxtFormatter<GameStr>> gameLogOutput("game.log");

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
		selfInfo->name = "EDF5ModLoader";
		selfInfo->version = PLUG_VER(1, 0, 5, 1);
		PluginData *selfData = new PluginData;
		selfData->info = selfInfo;
		selfData->module = hModule;
		plugins.push_back(selfData);

		PluginVersion v = selfInfo->version;
		PLOG_INFO.printf("EDF5ModLoader v%u.%u.%u Initializing\n", v.major, v.minor, v.patch);

		// Setup DLL proxy
		wchar_t path[MAX_PATH];
		if (!GetWindowsDirectoryW(path, _countof(path))) {
			DWORD dwError = GetLastError();
			PLOG_ERROR << "Failed to get windows directory path: error " << dwError;
			ExitProcess(EXIT_FAILURE);
		}

		wcscat_s(path, L"\\System32\\winmm.dll");

		PLOG_INFO << "Loading real winmm.dll";
		PLOG_INFO << "Setting up dll proxy functions";
		setupFunctions(LoadLibraryW(path));

		// Create ModLoader folders
		CreateDirectoryW(L"Mods", NULL);
		CreateDirectoryW(L"Mods\\Plugins", NULL);

		// Setup hooks
		HMODULE hmodEXE = GetModuleHandleW(NULL);

		// Hook function for additional ModLoader initialization
		PLOG_INFO << "Hooking EDF5.exe+3d8f00 (Additional initialization)";
		fnk3d8f00_orig = (fnk3d8f00_func)((PBYTE)hmodEXE + 0x3d8f00);
		if (!SetHookWrap(fnk3d8f00_hook, reinterpret_cast<PVOID*>(&fnk3d8f00_orig))) {
			// Error
			PLOG_ERROR << "Failed to setup EDF5.exe+3d8f00 hook";
		}

		// Add Mods folder redirector hook
		if (Redirect) {
			PLOG_INFO << "Hooking EDF5.exe+27380 (Mods folder redirector)";
			fnk27380_orig = (fnk27380_func)((PBYTE)hmodEXE + 0x27380);
			if (!SetHookWrap(fnk27380_hook, reinterpret_cast<PVOID*>(&fnk27380_orig))) {
				// Error
				PLOG_ERROR << "Failed to setup EDF5.exe+27380 hook";
			}
		} else {
			PLOG_INFO << "Skipping EDF5.exe+3e420 hook (Mods folder redirector)";
		}

		// Extra Mods folder redirector hook
		if (Redirect) {
			PLOG_INFO << "Hooking EDF5.exe+3e420 (Mods folder redirector)";
			fnk3e420_orig = (fnk3e420_func)((PBYTE)hmodEXE + 0x3e420);
			if (!SetHookWrap(fnk3e420_hook, reinterpret_cast<PVOID*>(&fnk3e420_orig))) {
				// Error
				PLOG_ERROR << "Failed to setup EDF5.exe+3e420 hook";
			}
		}

		// Add internal logging hook
		if (GameLog) {
			PLOG_INFO << "Hooking EDF5.exe+27680 (Interal logging hook)";
			fnk27680_orig = (fnk27680_func)((PBYTE)hmodEXE + 0x27680);
			if (!SetHookWrap(fnk27680_hook, reinterpret_cast<PVOID*>(&fnk27680_orig))) {
				// Error
				PLOG_ERROR << "Failed to setup EDF5.exe+27680 hook";
			}
		} else {
			PLOG_INFO << "Skipping EDF5.exe+27680 hook (Interal logging hook)";
		}

		// Finished
		PLOG_INFO << "Basic initialization complete";

		break;
	}
	case DLL_PROCESS_DETACH: {
		PLOG_INFO << "EDF5ModLoader Unloading";

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
