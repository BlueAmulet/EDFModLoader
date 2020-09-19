#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include <PluginAPI.h>

// Quick logging routines in absence of shared logging api
static FILE *hLogFile;

#define ltlog(func, ...)    do { LogDate(); func(__VA_ARGS__); fflush(hLogFile); } while (0)

#define ltprintf(fmt, ...)  ltlog(fprintf, hLogFile, fmt "\n", __VA_ARGS__);
#define ltputs(str)         ltlog(fputs, str "\n", hLogFile);
#define ltputws(str)        ltlog(fuputs, str L"\n", hLogFile);
#define ltwprintf(fmt, ...) ltlog(fuprintf, hLogFile, fmt L"\n", __VA_ARGS__);

#define lbprintf(...) fprintf(hLogFile, __VA_ARGS__)

#define lputs(...) do { fputs(__VA_ARGS__, hLogFile); fflush(hLogFile); } while (0)

static void LogDate(void) {
	SYSTEMTIME lt;
	GetLocalTime(&lt);

	lbprintf("[%04d-%02d-%02d %02d:%02d:%02d.%03d] ", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);
}

static int fuputs(const wchar_t *str, FILE *file) {
	int wstr_len = (int)wcslen(str);
	int num_chars = WideCharToMultiByte(CP_UTF8, 0, str, wstr_len, NULL, 0, NULL, NULL);
	PCHAR strTo = (PCHAR)HeapAlloc(GetProcessHeap(), 0, ((SIZE_T)num_chars + 1) * sizeof(CHAR));
	if (strTo != NULL) {
		WideCharToMultiByte(CP_UTF8, 0, str, wstr_len, strTo, num_chars, NULL, NULL);
		strTo[num_chars] = '\0';
		int rc = fputs(strTo, file);
		HeapFree(GetProcessHeap(), 0, strTo);
		return rc;
	} else {
		fputs("(Memory allocation failure)", file);
	}
	return 0; // TODO: What to return?
}

static int fuprintf(FILE *file, const wchar_t *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int required = _vsnwprintf(NULL, 0, fmt, args);
	wchar_t *buffer = new wchar_t[(size_t)required + 1];
	int rc = _vsnwprintf(buffer, (size_t)required + 1, fmt, args);
	va_end(args);
	fuputs(buffer, file);
	delete[] buffer;
	return rc;
}

// Parsed patch record
typedef struct {
	uint64_t offset;
	unsigned char *bytes;
	size_t length;
} PatchRecord;

enum struct PatchType {
	kInteger,
	kFloat,
	kDouble
};

// Injects patches into game process
static void WriteBuffer(void *addr, void *data, size_t len) {
	DWORD oldProtect;
	VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(addr, data, len);
	VirtualProtect(addr, len, oldProtect, &oldProtect);
}

// Remove whitespace and configurable data indicator (brackets)
static int patchfilter(int c) {
	return isspace(c) || c == '[' || c == ']';
}

extern "C" {
BOOL __declspec(dllexport) EML5_Load(PluginInfo *pluginInfo) {
	// Patcher does not need to remain loaded, so not filling PluginInfo
	hLogFile = fopen("Patcher.log", "wb");
	WIN32_FIND_DATAW ffd;
	std::fstream pfile;
	UINT_PTR hmodEXE = (UINT_PTR)GetModuleHandleW(NULL);

	ltputs("EDF5 Patcher v1.0.2");
	ltputs("Loading patches");
	HANDLE hFind = FindFirstFileW(L"Mods\\Patches\\*.txt", &ffd);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				ltwprintf(L"Loading patch: %s", ffd.cFileName);
				wchar_t patchPath[MAX_PATH];
				wcscpy_s(patchPath, L"Mods\\Patches\\");
				wcscat_s(patchPath, ffd.cFileName);
				pfile.open(patchPath, std::ios::in);
				if (pfile.is_open()) {
					std::string patchInput;
					bool patch = true;
					std::vector<PatchRecord*> patches;
					while (getline(pfile, patchInput)) {
						// Clean line of whitespace
						patchInput.erase(std::remove_if(patchInput.begin(), patchInput.end(), patchfilter), patchInput.end());

						// Remove comments
						size_t scpos = patchInput.find(";");
						if (scpos != std::string::npos) {
							patchInput = patchInput.substr(0, scpos);
						}

						// Ignore empty lines
						if (!patchInput.empty()) {
							// Check for colon
							size_t cpos = patchInput.find(":");
							if (cpos != std::string::npos) {
								// Split input into two parts
								std::string address = patchInput.substr(0, cpos);
								std::string patchData = patchInput.substr(cpos + 1);
								// Check for type delimiter
								size_t tpos = patchData.find("!");
								if (tpos != std::string::npos) {
									std::string patchType = patchData.substr(0, tpos);
									std::string patchValue = patchData.substr(tpos + 1);
									// Validate patch input
									if (address.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos) {
										ltputs("Malformed patch input: Non hexadecimal characters in address");
										patch = false;
									} else if (patch) {
										PatchType pType;
										size_t iLimitMax;
										SSIZE_T iLimitMin = 0;
										size_t iLength;
										bool needValue = false;
										if (patchType == "float" || patchType == "f32") {
											pType = PatchType::kFloat;
											iLength = sizeof(float);
											needValue = true;
										} else if (patchType == "double" || patchType == "f64") {
											pType = PatchType::kDouble;
											iLength = sizeof(double);
											needValue = true;
										} else if (patchType.length() >= 2 && (patchType[0] == 's' || patchType[0] == 'u' || patchType[0] == 'n' || patchType[0] == 'p')) {
											pType = PatchType::kInteger;
											needValue = true;
											std::string patchLength = patchType.substr(1);
											if (patchLength == "8") {
												iLength = 1;
												iLimitMax = 0xFF;
											} else if (patchLength == "16") {
												iLength = 2;
												iLimitMax = 0xFFFF;
											} else if (patchLength == "32") {
												iLength = 4;
												iLimitMax = 0xFFFFFFFF;
											} else if (patchLength == "64") {
												iLength = 8;
												iLimitMax = 0xFFFFFFFFFFFFFFFF;
											} else {
												ltprintf("Unknown patch type: %s", patchType.c_str());
												patch = false;
											}
										} else {
											ltprintf("Unknown patch type: %s", patchType.c_str());
											patch = false;
										}
										if (patch && needValue && patchValue.empty()) {
											ltprintf("Malformed patch input: Patch type %s requires additional value", patchType.c_str());
											patch = false;
										}
										if (patch) {
											// Decode patch value
											unsigned char *bytes = NULL;
											char *endPtr;
											if (pType == PatchType::kFloat) {
												float value = strtof(patchValue.c_str(), &endPtr);
												if (endPtr == patchValue.c_str() + patchValue.size()) {
													bytes = new unsigned char[iLength];
													memcpy(bytes, &value, iLength);
												} else {
													ltputs("Malformed patch input: Patch value could not be parsed");
													patch = false;
												}
											} else if (pType == PatchType::kDouble) {
												double value = strtod(patchValue.c_str(), &endPtr);
												if (endPtr == patchValue.c_str() + patchValue.size()) {
													bytes = new unsigned char[iLength];
													memcpy(bytes, &value, iLength);
												} else {
													ltputs("Malformed patch input: Patch value could not be parsed");
													patch = false;
												}
											} else if (pType == PatchType::kInteger) {
												if (patchType[0] != 'u') {
													// Correct limits
													iLimitMax /= 2;
													iLimitMin = -((SSIZE_T)(iLimitMax)) - 1;
													if (patchType[0] == 'p') { // Signed positive
														iLimitMin = 0;
													} else if (patchType[0] == 'n') { // Signed negative
														iLimitMax = 0;
													}
													SSIZE_T value = strtoll(patchValue.c_str(), &endPtr, 0);
													if (endPtr != patchValue.c_str() + patchValue.size()) {
														ltputs("Malformed patch input: Patch value could not be parsed");
														patch = false;
													} else if (value > (SSIZE_T)iLimitMax || value < iLimitMin) {
														ltprintf("Value limits exceeded: (%I64d, %I64u): %s", iLimitMin, iLimitMax, patchValue.c_str());
														patch = false;
													} else {
														bytes = new unsigned char[iLength];
														memcpy(bytes, &value, iLength);
													}
												} else {
													size_t value = strtoull(patchValue.c_str(), &endPtr, 0);
													if (endPtr != patchValue.c_str() + patchValue.size()) {
														ltputs("Malformed patch input: Patch value could not be parsed");
														patch = false;
													} else if (value > iLimitMax || patchValue[0] == '-') {
														ltprintf("Value limits exceeded: (%I64d, %I64u): %s", iLimitMin, iLimitMax, patchValue.c_str());
														patch = false;
													} else {
														bytes = new unsigned char[iLength];
														memcpy(bytes, &value, iLength);
													}
												}
											}
											if (patch) {
												// Add patch to list
												PatchRecord *record = new PatchRecord;
												record->offset = strtoull(address.c_str(), NULL, 16);
												record->length = iLength;
												record->bytes = bytes;
												patches.push_back(record);
											}
										}
									}
								} else {
									// Validate patch input
									if (address.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos) {
										ltputs("Malformed patch input: Non hexadecimal characters in address");
										patch = false;
									} else if (patchData.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos) {
										ltputs("Malformed patch input: Non hexadecimal characters in patch data");
										patch = false;
									} else if (patchData.size() % 2 != 0) {
										ltprintf("Incomplete patch data for address %s", address.c_str());
										patch = false;
									} else if (patch) {
										// Parse patch input
										size_t hsize = patchData.size() / 2;
										unsigned char *bytes = new unsigned char[hsize];
										char hexPart[3];
										hexPart[2] = '\0';
										for (size_t i = 0; i < patchData.size(); i += 2) {
											hexPart[0] = patchData[i];
											hexPart[1] = patchData[i + 1];
											bytes[i / 2] = (unsigned char)strtoul(hexPart, NULL, 16);
										}
										PatchRecord *record = new PatchRecord;
										record->offset = strtoull(address.c_str(), NULL, 16);
										record->bytes = bytes;
										record->length = hsize;
										patches.push_back(record);
									}
								}
							} else {
								ltputs("Malformed patch input: Missing colon");
							}
						}
					}
					if (patch) {
						// Apply memory patches
						if (!patches.empty()) {
							for (PatchRecord *record : patches) {
								ltprintf("Patching %I64d bytes at EDF5.exe+%I64x", record->length, record->offset);
								WriteBuffer((void*)(hmodEXE + record->offset), record->bytes, record->length);
							}
							lputs("\n");
						} else {
							ltputs("No patches found in file\n");
						}
					} else {
						ltputs("Ignoring patch\n");
					}
					// Clean up memory
					for (PatchRecord *precord : patches) {
						delete[] precord->bytes;
						delete precord;
					}
					patches.clear();
					pfile.close();
				} else {
					ltputs("Failed to open patch file\n");
				}
			}
		} while (FindNextFileW(hFind, &ffd) != 0);
		// Check if finished with error
		DWORD dwError = GetLastError();
		if (dwError != ERROR_NO_MORE_FILES) {
			ltprintf("Failed to search for patches: error %lu", dwError);
		}
		FindClose(hFind);
	} else {
		DWORD dwError = GetLastError();
		if (dwError != ERROR_FILE_NOT_FOUND && dwError != ERROR_PATH_NOT_FOUND) {
			ltprintf("Failed to search for patches: error %lu", dwError);
		}
	}

	// Close log file
	fclose(hLogFile);

	return false; // Unload plugin
}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
