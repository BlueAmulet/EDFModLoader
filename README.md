# EDFModLoader

A very basic rudimentary modloader for Earth Defense Force 4.1 & 5.  
Supports automatic Root.cpk redirection and DLL plugin loading.  
Writes internal game logging to game.log

This repository contains submodules! Please use `--recurse-submodules` when cloning.

## Installation
Get the latest package from [Releases](https://github.com/BlueAmulet/EDFModLoader/releases) and unpack it in the same folder as EDF5.exe

https://github.com/BlueAmulet/EDFModLoader/releases

## Plugins
### Patcher
[Patcher](https://github.com/BlueAmulet/EDFModLoader/blob/master/Patcher/README.md) is a plugin to perform runtime memory patches.  
It loads .txt files containing patch data from `Mods\Patches`, see [Patcher documentation](https://github.com/BlueAmulet/EDFModLoader/blob/master/Patcher/README.md) for more information.  
Patches by [Souzooka](https://github.com/Souzooka) are included by default.  
Additional patches can be found in the `ExtraPatches` folder, they should be moved to `Mods\Patches` to be used.  

### Making your own
The Plugin API is in `PluginAPI.h` and is currently unfinished and subject to change.  
Plugins should export a function of type `bool __fastcall EML5_Load(PluginInfo*)`  
Return true to remain loaded in memory, and false to unload in case of error or desired behavior.  
If your plugin remains in memory, fill out the PluginInfo struct:  
```
pluginInfo->infoVersion = PluginInfo::MaxInfoVer;
pluginInfo->name = "Plugin Name";
pluginInfo->version = PLUG_VER(Major, Minor, Patch, Build);
```

## Building
You will need [Visual Studio 2019](https://visualstudio.microsoft.com/vs/community/) and [vcpkg](https://github.com/microsoft/vcpkg)

To setup vcpkg and required libraries:  
```
git clone https://github.com/microsoft/vcpkg
cd vcpkg
bootstrap-vcpkg.bat
vcpkg install zydis:x64-windows-static-md plog:x64-windows-static-md
```

EARTH DEFENSE FORCE is the registered trademark of SANDLOT and D3 PUBLISHER INC. This project is not affiliated with or endorsed by SANDLOT or D3 PUBLISHER INC in any way.
