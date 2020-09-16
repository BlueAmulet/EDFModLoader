#pragma once

#ifdef __cplusplus
extern "C" {
#endif

union PluginVersion {
	struct {
		unsigned short build;
		unsigned short patch;
		unsigned short minor;
		unsigned short major;
	};
	unsigned long long raw;
};
static_assert(sizeof(PluginVersion) == 8, "PluginVersion union has unexpected size");

// Helper macro to fill out the PluginVersion field
#define PLUG_VER(a, b, c, d) {{d, c, b, a}}

typedef struct {
	enum { MaxInfoVer = 1 };
	unsigned long infoVersion;
	const char *name;
	PluginVersion version;
} PluginInfo;
static_assert(sizeof(PluginInfo) == 24, "PluginInfo struct has unexpected size");

#ifdef __cplusplus
} // extern "C"
#endif