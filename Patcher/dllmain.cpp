#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include <LightningScanner/LightningScanner.hpp>

#include <PluginAPI.h>

#include <windows.h>
#include <shlwapi.h>
#include <psapi.h>

// Quick logging routines in absence of shared logging api
static FILE *hLogFile;

#define ltlog(func, ...)    do { LogDate(); func(__VA_ARGS__); fflush(hLogFile); } while (0)

#define ltprintf(fmt, ...)  ltlog(fprintf, hLogFile, fmt "\n", __VA_ARGS__)
#define ltputs(str)         ltlog(fputs, str "\n", hLogFile)
#define ltputws(str)        ltlog(fuputs, str L"\n", hLogFile)
#define ltwprintf(fmt, ...) ltlog(fuprintf, hLogFile, fmt L"\n", __VA_ARGS__)

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
	uintptr_t offset;
	unsigned char *bytes;
	size_t length;
} PatchRecord;

enum struct PatchType {
	kInteger,
	kFloat,
	kDouble,
	kRelative
};

// Injects patches into game process
static void WriteBuffer(void *addr, void *data, size_t len) {
	DWORD oldProtect;
	VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(addr, data, len);
	VirtualProtect(addr, len, oldProtect, &oldProtect);
}

// Consistent locale independant definition of whitespace
static bool iswhitespace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

static bool isnotspace(char c) {
	return !iswhitespace(c);
}

// Whitespace and configurable data indicator (brackets)
static bool patchfilter(char c) {
	return iswhitespace(c) || c == '[' || c == ']';
}

// Invalid characters for a label
static bool labelfilter(char c) {
	return iswhitespace(c) || c == ':' || c == '-' || c == '+';
}

static void trim_left(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), isnotspace));
}

static void trim_right(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), isnotspace).base(), s.end());
}

static void trim(std::string &s) {
	trim_left(s);
	trim_right(s);
}

static uint8_t CharToByteUnsafe(char symbol) {
	if (symbol >= 'a') {
		return symbol - 'a' + 0xA;
	} else if (symbol >= 'A') {
		return symbol - 'A' + 0xA;
	} else {
		return symbol - '0';
	}
}

void EMLCommon_Load(BOOL EDF6) {
	hLogFile = fopen("Patcher.log", "wb");
	ltputs("EDF Patcher v1.1.0");

	PBYTE hmodEXE;
	if (EDF6) {
		// TODO: Hack to support EDF6
		hmodEXE = (PBYTE)GetModuleHandleW(L"EDF.dll");
	} else {
		hmodEXE = (PBYTE)GetModuleHandleW(NULL);
	}
	if (hmodEXE == NULL)
	{
		if (EDF6) {
			ltputs("Failed to get handle to EDF.dll");
		} else {
			ltputs("Failed to get handle to game executable");
		}
		return;
	}
	char hmodName[MAX_PATH];
	GetModuleFileNameA((HMODULE)hmodEXE, hmodName, _countof(hmodName));
	char *hmodFName = PathFindFileNameA(hmodName);
	memmove(hmodName, hmodFName, strlen(hmodFName) + 1);

	// Determine bounds of executable
	MODULEINFO modInfo = {0};
	if (!GetModuleInformation(GetCurrentProcess(), (HMODULE)hmodEXE, &modInfo, sizeof(MODULEINFO))) {
		ltprintf("Failed to fetch size information for %s", hmodName);
		return;
	}

	SYSTEM_INFO sysInfo = {0};
	GetSystemInfo(&sysInfo);

	ltputs("Loading patches");
	std::fstream pfile;
	WIN32_FIND_DATAW ffd;
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
					bool patch = true;
					uintptr_t lastOffset = 0;
					char *endPtr;
					std::string patchInput;
					std::vector<PatchRecord*> patches;
					std::vector<void*> tempAlloc;
					std::unordered_map<std::string, uintptr_t> labels;
					while (getline(pfile, patchInput)) {
						// Remove comments
						size_t scpos = patchInput.find(';');
						if (scpos != std::string::npos) {
							patchInput = patchInput.substr(0, scpos);
						}

						// Trim whitespace
						trim(patchInput);

						// Ignore empty lines
						if (patchInput.empty()) {
							continue;
						}

						// Check for colon
						size_t cpos = patchInput.find(':');
						std::string addressStr;
						std::string patchData = patchInput;
						if (cpos != std::string::npos) {
							// Split input into two parts
							addressStr = patchInput.substr(0, cpos);
							trim_right(addressStr);

							// Verify address does not have whitespace, otherwise ignore
							if (!std::any_of(addressStr.begin(), addressStr.end(), iswhitespace)) {
								patchData = patchInput.substr(cpos + 1);
								trim_left(patchData);

								size_t opos;
								std::string address = addressStr;
								std::string offsetStr;
								bool negative;
								if ((opos = addressStr.find('+')) != std::string::npos) {
									offsetStr = addressStr.substr(opos + 1);
									address = addressStr.substr(0, opos);
									negative = false;
								} else if ((cpos = addressStr.find('-')) != std::string::npos) {
									offsetStr = addressStr.substr(opos + 1);
									address = addressStr.substr(0, opos);
									negative = true;
								}

								// Validate address
								std::unordered_map<std::string, uintptr_t>::const_iterator got = labels.find(address);
								if (got != labels.end()) {
									lastOffset = got->second;
								} else if (address.find_first_not_of("0123456789ABCDEFabcdef") == std::string::npos) {
									lastOffset = strtoull(address.c_str(), NULL, 16);
								} else {
									ltprintf("Malformed patch input: Undefined label: %s", address.c_str());
									patch = false;
								}

								// Add offset if present
								if (!offsetStr.empty()) {
									const char *offsetStrC = offsetStr.c_str();
									size_t offset = strtoull(offsetStrC, &endPtr, 16);
									if (endPtr == offsetStrC + offsetStr.size()) {
										if (negative) {
											lastOffset -= offset;
										} else {
											lastOffset += offset;
										}
									} else {
										ltprintf("Malformed patch input: Invalid address offset: %s", addressStr.c_str());
										patch = false;
									}
								}
							} else {
								// Probably a colon in the middle of a command, don't treat as an address
								addressStr.clear();
							}
						}
						if (!patchData.empty()) {
							// Check for commands
							std::string command = patchData;
							std::string argument;
							std::string::iterator spos = std::find_if(command.begin(), command.end(), iswhitespace);
							if (spos != command.end()) {
								argument = command.substr(spos - command.begin() + 1);
								command = command.substr(0, spos - command.begin());
								trim_left(argument);
							}

							if (command == "aob") {
								if (!argument.empty()) {
									// Split off the symbol from the rest of the command
									spos = std::find_if(argument.begin(), argument.end(), iswhitespace);
									if (spos != argument.end()) {
										std::string symbol = argument.substr(0, spos - argument.begin());
										std::string pattern = argument.substr(spos - argument.begin() + 1);
										trim_left(pattern);
										bool valid = true;

										// Validate symbol name
										if (std::find_if(symbol.begin(), symbol.end(), labelfilter) != symbol.end()) {
											ltprintf("Malformed patch input: Invalid label name: %s", symbol.c_str());
											valid = false;
										}

										// TODO: Validate pattern
										if (valid) {
											const auto scanner = LightningScanner::Scanner(LightningScanner::Pattern(pattern));
											void *result = scanner.Find(hmodEXE, modInfo.SizeOfImage).Get<void>();
											if (result != NULL) {
												labels[symbol] = (uintptr_t)result - (uintptr_t)hmodEXE;
												ltprintf("AOB scan found %s at %I64X", symbol.c_str(), labels[symbol]);
											} else {
												ltprintf("AOB scan failed: %s: %s", symbol.c_str(), pattern.c_str());
												patch = false;
											}
										} else {
											patch = false;
										}
									} else {
										ltputs("Malformed patch input: Command aob requires 2 arguments, 1 given");
										patch = false;
									}
								} else {
									ltputs("Malformed patch input: Command aob requires 2 arguments, 0 given");
									patch = false;
								}
							} else if (command == "alloc") {
								if (!argument.empty()) {
									// Split arguments up by whitespace
									std::vector<std::string> arguments;
									std::string::iterator start = argument.begin();
									do {
										std::string::iterator idx = std::find_if(start, argument.end(), iswhitespace);
										if (idx == argument.end()) {
											break;
										}
										if (idx != start) {
											arguments.push_back(argument.substr(start - argument.begin(), idx - start));
										}
										start = idx + 1;
									} while (true);
									arguments.push_back(argument.substr(start - argument.begin()));

									if (arguments.size() < 2) {
										ltputs("Malformed patch input: Command alloc requires at least 2 arguments, 1 given");
										patch = false;
									} else if (arguments.size() > 3) {
										ltprintf("Malformed patch input: Command alloc takes at most 3 arguments, %I64u given", arguments.size());
										patch = false;
									} else {
										bool valid = true;
										void *target = hmodEXE;

										// Validate label name
										if (std::find_if(arguments[0].begin(), arguments[0].end(), labelfilter) != arguments[0].end()) {
											ltprintf("Malformed patch input: Invalid label name: %s", arguments[0].c_str());
											valid = false;
										}

										// Parse allocation size
										const char *sizeStr = arguments[1].c_str();
										size_t allocSize = strtoull(sizeStr, &endPtr, 0);
										if (endPtr != sizeStr + arguments[1].size()) {
											ltprintf("Malformed patch input: Invalid allocation size: %s", sizeStr);
											valid = false;
										}

										// Parse target address if present
										if (arguments.size() == 3) {
											std::unordered_map<std::string, uintptr_t>::const_iterator got = labels.find(arguments[2]);
											if (got != labels.end()) {
												target = hmodEXE + got->second;
											} else {
												const char *targetAddr = arguments[2].c_str();
												size_t value = strtoull(targetAddr, &endPtr, 16);
												if (endPtr == targetAddr + arguments[2].size()) {
													target = hmodEXE + value;
												} else {
													ltprintf("Malformed patch input: Invalid allocation address: %s", targetAddr);
													valid = false;
												}
											}
										}
										if (valid) {
											if (patch) {
												void *alloc = NULL;
												bool forward = false;
												void *originalTarget = target;
												// Attempt to find an address close to our target
												do {
													alloc = VirtualAlloc(target, allocSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
													if (alloc == NULL) {
														MEMORY_BASIC_INFORMATION mbi = {};
														if (VirtualQuery(target, &mbi, sizeof(MEMORY_BASIC_INFORMATION))) {
															if (forward) {
																target = (PBYTE)mbi.BaseAddress + mbi.RegionSize;
															} else {
																target = (PBYTE)mbi.BaseAddress - sysInfo.dwAllocationGranularity;
															}
														} else {
															if (!forward) {
																// Try again moving forward this time
																forward = true;
																target = originalTarget;
															} else {
																break;
															}
														}
													}
												} while (alloc == NULL);
												if (alloc != NULL) {
													ltprintf("Allocated %s bytes at %p", sizeStr, alloc);
													labels[arguments[0]] = (uintptr_t)alloc - (uintptr_t)hmodEXE;
													tempAlloc.push_back(alloc);
												} else {
													ltprintf("Allocation failed: %s bytes for %s", sizeStr, arguments[0].c_str());
													patch = false;
												}
											} else {
												// Patch failed, but create dummy label anyway to reduce future errors
												labels[arguments[0]] = (uintptr_t)target - (uintptr_t)hmodEXE;
											}
										} else {
											patch = false;
										}
									}
								} else {
									ltputs("Malformed patch input: Command alloc requires at least 2 arguments, 0 given");
									patch = false;
								}
							} else if (command == "label") {
								if (argument.empty()) {
									ltputs("Malformed patch input: Command label requires 1 argument, 0 given");
									patch = false;
								} else if (std::find_if(argument.begin(), argument.end(), labelfilter) != argument.end()) {
									ltprintf("Malformed patch input: Invalid label name: %s", argument.c_str());
									patch = false;
								} else {
									labels[argument] = lastOffset;
								}
							} else {
								// Check for type delimiter
								size_t tpos = patchData.find('!');
								if (tpos != std::string::npos) {
									std::string patchType = patchData.substr(0, tpos);
									std::string patchValue = patchData.substr(tpos + 1);
									trim_right(patchType);
									trim_left(patchValue);
									PatchType pType;
									size_t iLimitMax;
									SSIZE_T iLimitMin = 0;
									size_t iLength;
									bool needValue = false;

									// Validate patch input
									if (patchType == "float" || patchType == "f32") {
										pType = PatchType::kFloat;
										iLength = sizeof(float);
										needValue = true;
									} else if (patchType == "double" || patchType == "f64") {
										pType = PatchType::kDouble;
										iLength = sizeof(double);
										needValue = true;
									} else if (patchType == "rel32") {
										pType = PatchType::kRelative;
										iLength = 4;
										needValue = true;
									} else if (patchType == "rel64") {
										pType = PatchType::kRelative;
										iLength = 8;
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
										const char *patchValueC = patchValue.c_str();
										if (pType == PatchType::kFloat) {
											float value = strtof(patchValueC, &endPtr);
											if (endPtr == patchValueC + patchValue.size()) {
												bytes = new unsigned char[iLength];
												memcpy(bytes, &value, iLength);
											} else {
												ltprintf("Malformed patch input: Patch value could not be parsed: %s", patchData.c_str());
												patch = false;
											}
										} else if (pType == PatchType::kDouble) {
											double value = strtod(patchValueC, &endPtr);
											if (endPtr == patchValueC + patchValue.size()) {
												bytes = new unsigned char[iLength];
												memcpy(bytes, &value, iLength);
											} else {
												ltprintf("Malformed patch input: Patch value could not be parsed: %s", patchData.c_str());
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
												SSIZE_T value = strtoll(patchValueC, &endPtr, 0);
												if (endPtr != patchValueC + patchValue.size()) {
													ltprintf("Malformed patch input: Patch value could not be parsed: %s", patchData.c_str());
													patch = false;
												} else if (value > (SSIZE_T)iLimitMax || value < iLimitMin) {
													ltprintf("Value limits exceeded: (%I64d, %I64u): %s", iLimitMin, iLimitMax, patchValueC);
													patch = false;
												} else {
													bytes = new unsigned char[iLength];
													memcpy(bytes, &value, iLength);
												}
											} else {
												size_t value = strtoull(patchValueC, &endPtr, 0);
												if (endPtr != patchValueC + patchValue.size()) {
													ltprintf("Malformed patch input: Patch value could not be parsed: %s", patchData.c_str());
													patch = false;
												} else if (value > iLimitMax || patchValue[0] == '-') {
													ltprintf("Value limits exceeded: (%I64d, %I64u): %s", iLimitMin, iLimitMax, patchValueC);
													patch = false;
												} else {
													bytes = new unsigned char[iLength];
													memcpy(bytes, &value, iLength);
												}
											}
										} else if (pType == PatchType::kRelative) {
											if (std::find_if(argument.begin(), argument.end(), iswhitespace) == argument.end()) {
												uintptr_t targetAddr;
												std::unordered_map<std::string, uintptr_t>::const_iterator got = labels.find(patchValue);
												if (got != labels.end()) {
													targetAddr = got->second;
												} else if (patchValue.find_first_not_of("0123456789ABCDEFabcdef") == std::string::npos) {
													targetAddr = strtoull(patchValue.c_str(), NULL, 16);
												} else {
													ltprintf("Malformed patch input: Undefined label: %s", patchValue.c_str());
													patch = false;
												}
												if (patch) {
													ptrdiff_t offset = targetAddr - lastOffset - iLength;
													if (iLength == 4) {
														// Verify relative address fits within 32bits
														if (offset > INT32_MAX || offset < INT32_MIN) {
															ltprintf("Relative offset too large: %I64x - %I64x", targetAddr, lastOffset);
															patch = false;
														}
													}
													if (patch) {
														bytes = new unsigned char[iLength];
														memcpy(bytes, &offset, iLength);
													}
												}
											} else {
												ltprintf("Malformed patch input: Labels cannot contain whitespace: %s", patchValue.c_str());
												patch = false;
											}
										}
										if (patch) {
											// Add patch to list
											PatchRecord *record = new PatchRecord;
											record->offset = lastOffset;
											record->length = iLength;
											record->bytes = bytes;
											patches.push_back(record);
											lastOffset += iLength;
										}
									}
								} else {
									// Strip uninportant characters from patch data
									patchData.erase(std::remove_if(patchData.begin(), patchData.end(), patchfilter), patchData.end());
									// Validate patch input
									if (patchData.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos) {
										ltputs("Malformed patch input: Non hexadecimal characters in patch data");
										patch = false;
									} else if (patchData.size() % 2 != 0) {
										if (!addressStr.empty()) {
											ltprintf("Malformed patch input: Incomplete patch data for address %s", addressStr.c_str());
										} else {
											ltprintf("Malformed patch input: Incomplete patch data at %I64X", lastOffset);
										}
										patch = false;
									} else if (patch) {
										// Parse patch input
										size_t hsize = patchData.size() / 2;
										unsigned char *bytes = new unsigned char[hsize];
										for (size_t i = 0; i < patchData.size(); i += 2) {
											bytes[i / 2] = CharToByteUnsafe(patchData[i]) << 4 | CharToByteUnsafe(patchData[i + 1]);
										}
										PatchRecord *record = new PatchRecord;
										record->offset = lastOffset;
										record->bytes = bytes;
										record->length = hsize;
										patches.push_back(record);
										lastOffset += hsize;
									}
								}
							}
						}
					}
					if (patch) {
						// Apply memory patches
						if (!patches.empty()) {
							for (PatchRecord *record : patches) {
								if (record->offset >= modInfo.SizeOfImage) {
									ltprintf("Patching %I64d bytes at %p", record->length, hmodEXE + record->offset);
								} else {
									ltprintf("Patching %I64d bytes at %s+%I64X", record->length, hmodName, record->offset);
								}
								WriteBuffer(hmodEXE + record->offset, record->bytes, record->length);
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
}

extern "C" {
// Patcher does not need to remain loaded, so not filling PluginInfo
BOOL __declspec(dllexport) EML4_Load(PluginInfo *pluginInfo) {
	EMLCommon_Load(false);
	return false;
}

BOOL __declspec(dllexport) EML5_Load(PluginInfo *pluginInfo) {
	EMLCommon_Load(false);
	return false;
}

BOOL __declspec(dllexport) EML6_Load(PluginInfo *pluginInfo) {
	EMLCommon_Load(true);
	return false;
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
