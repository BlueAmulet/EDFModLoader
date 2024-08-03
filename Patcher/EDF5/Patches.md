## Default Patches
These patches are included and active by default.  

### IncreaseChatLimit (by BlueAmulet, based on Souzooka's patch)
IncreaseChatLimit increases the chat character limit from 32 to 128 characters.  
The limit can be increased up to 255 if desired.  
Based on this [guide](https://steamcommunity.com/sharedfiles/filedetails/?id=1814868040) by Souzooka, variant by BlueAmulet  

### RemoveChatCensor (by BlueAmulet, based on Souzooka's patch)
RemoveChatCensor removes chat censorship on your messages, and the messages that other players post.  
Other players will still see your message as censored unless they also have this patch.  
Based on this [guide](https://steamcommunity.com/sharedfiles/filedetails/?id=1814868040) by Souzooka, variant by BlueAmulet  

### InvalidMissionIDFix (by Souzooka)
Fixes a crash when browsing the Online lobby if any rooms have a modded mission ID that is out of bounds for your game.  
Based on this [guide](https://github.com/KCreator/Earth-Defence-Force-Documentation/wiki/Useful-edits-to-EDF-5-game-executable#bugfix-to-mission-list-error) by Souzooka, originally posted in the EDF discord.  

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

### ChangeFOV (by Souzooka)
ChangeFOV lets you change the game's FOV from it's default of 45 vFOV.  
Various presets are included in the ChangeFOV.txt file, see reddit post for image previews.  
Based on this [reddit post](https://reddit.com/r/EDF/comments/d7mjhw/has_there_been_an_fov_hack_yet/f17bbwn/) by Souzooka, ported to Patcher and added values for other FOV values.  

### ChangePickupLimits (by BlueAmulet)
Allows you to change how many boxes can be on a map at once, and how many boxes maximum can be collected from a map.  
