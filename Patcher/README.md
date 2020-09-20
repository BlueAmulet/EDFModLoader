# Patcher Plugin

Patcher is a plugin to perform runtime memory patches.  
It loads .txt files containing patch data from `Mods\Patches`  
Patches by [Souzooka](https://github.com/Souzooka) are included by default.  
Additional patches can be found in the `ExtraPatches` folder, they should be moved to `Mods\Patches` to be used.  

## Installation
Get the latest Plugins.zip package from [Releases](https://github.com/BlueAmulet/EDF5ModLoader/releases) and unpack it in the same folder as EDF5.exe

https://github.com/BlueAmulet/EDF5ModLoader/releases

## Format
Patcher accepts .txt files in `Mods\Patches` of the format:  
```
Offset: Hex bytes ; Optional comment
Offset: Type! Value ; Optional comment
```  
Where 'Offset' is a hexadecimal offset in memory from the base address of EDF5.exe  
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

## Default Patches
These patches are included and active by default.  

### IncreaseChatLimit (by BlueAmulet, based on Souzooka's patch)
IncreaseChatLimit increases the chat character limit from 32 to 128 characters.  
The limit can be increased up to 255 if desired.  
Based on this [guide](https://steamcommunity.com/sharedfiles/filedetails/?id=1814868040) by Souzooka, variant by BlueAmulet  

### RemoveChatCensor (by Souzooka)
RemoveChatCensor removes chat censorship on your messages, and the messages that other players post.  
Other players will still see your message as censored unless they also have this patch.  
Based on this [guide](https://steamcommunity.com/sharedfiles/filedetails/?id=1814868040) by Souzooka, ported to Patcher by BlueAmulet  

### MouseJitterFix (by Souzooka)
MouseJitterFix reduces mouse jerkiness for those suffering from the "mouse jittering" bug.  
This current solution does not solve the issue, but removes >99.5% of jittering.  
Jittering still seems to occur very rarely on its own, or in some cases where the user is clicking a lot (e.g. with an autoclicker) or has low performance.  
Based on this [guide](https://steamcommunity.com/sharedfiles/filedetails/?id=1819748441) by Souzooka, ported to Patcher by BlueAmulet  

## Extra Patches
These patches are included in the `ExtraPatches` folder and are not active by default.  
If you want to use any of these patches, move the respective patch into your `Mods\Patches` folder.

### BorderlessWindow (by BlueAmulet)
BorderlessWindow makes the game run in a borderless window.  
Can be used as an lite alternative to SpecialK.  

### NoAmbientOcclusion (by BlueAmulet)
NoAmbientOcclusion removes the game's ambient occlusion passes.  
This will remove the intense dark pixelated outlines on objects.  
