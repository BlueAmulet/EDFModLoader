# Patcher Plugin

Patcher is a plugin to perform runtime memory patches.  
It loads .txt files containing patch data from `Mods\Patches`  
Patches by [Souzooka](https://github.com/Souzooka) are included by default.  
Additional patches can be found in the `ExtraPatches` folder, they should be moved to `Mods\Patches` to be used.  

## Installation
Get the latest Plugins.zip package from [Releases](https://github.com/BlueAmulet/EDFModLoader/releases) and unpack it in the same folder as EDF41.exe or EDF5.exe respectively

https://github.com/BlueAmulet/EDFModLoader/releases

## Format
Patcher accepts .txt files in `Mods\Patches` of the format:  
```
Offset: Hex bytes ; Optional comment
Offset: Type! Value ; Optional comment
```  
Where 'Offset' is a hexadecimal offset from the in memory base address of the game.  
And 'Hex bytes' are a series of hexadecimal bytes to patch into that address.  
If an exclamation mark is found, the 'Type! Value' format is used instead of the 'Hex bytes' format.  
All data including and following a semicolon is ignored up to the end of the line.  

The following types are supported:  
| Type | Description |
|-|-|
| u# | Unsigned integer, # can be 8, 16, 32, or 64 |
| s# | Signed integer |
| p# | Positive signed integer |
| n# | Negative signed integer |
| float | 32bit floating point number |
| f32 | Alternative type name for float |
| double | 64bit floating point number |
| f64 | Alternative type name for double |

## Included Patches
[EDF4.1 Patches](https://github.com/BlueAmulet/EDFModLoader/blob/master/Patcher/EDF41/Patches.md)  
[EDF5 Patches](https://github.com/BlueAmulet/EDFModLoader/blob/master/Patcher/EDF5/Patches.md)
