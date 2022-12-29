#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

void setupFunctions(HMODULE);
void cleanupProxy(void);

#ifdef __cplusplus
} // extern "C"
#endif