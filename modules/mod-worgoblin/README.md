# Updates

2023-09-13 - @ben-of-codecraft
* Updated multiple files in DBCs to make this patch compatible with HD Patch. 
* This patch must load post the HD patch, and patches load Z>A 9-1 (with 1-3 as expansion patches) This means if your HD Patch is A, then this patch has to be B-Z. Other than that follow the 
instructions form the original patch below. 


# Worgoblin Module

[![core-build](https://github.com/benjymansy123/mod-worgoblin/actions/workflows/core-build.yml/badge.svg)](https://github.com/benjymansy123/mod-worgoblin/actions/workflows/core-build.yml)

This is a module for [AzerothCore](http://www.azerothcore.org) that adds worgen, goblins, and numerous features related to their playability.

## Important Notes

This module makes *heavy* use of client patching and modification. Because of this, it is recommended to use a [clean, unmodified enUS WoW client](https://www.chromiecraft.com/downloads) as issues have been reported due to use of other clients, such as the one offered from Warmane. The HD patch included on the ChromieCraft website has been known to cause issues with this patch. It is for that reason that it's recommended for you to use the HD patch adapted to this module in particular, which can be found [here](https://github.com/benjymansy123/mod-worgoblin/releases/tag/hd-patch) with instructions on how to install it.

## How to Install

As of right now, the hooks for allowing custom races is not present in AzerothCore. Because of this, you'll have to manually apply the change required to your AzerothCore source.

### 1) Apply the [worgoblin.patch](https://github.com/benjymansy123/mod-worgoblin/blob/master/worgoblin.patch) to your AzerothCore source directory.

This can be accomplished in a couple different ways. You could do it using Git by using the following command:

`git apply --ignore-space-change --ignore-whitespace modules/mod-worgoblin/worgoblin.patch`

Or you can do it manually through a text editor of your choice by changing the lines required where a "-" indicates the original line and a "+" indicates the change necessary. Where to find the specific lines can be found after the @@ before each change, where it states the line number. There are a total of seven lines to change and one line to add.

### 2) Replace the DBC files in your AzerothCore Data directory with the ones provided in [DBFilesClient](https://github.com/benjymansy123/mod-worgoblin/tree/master/patch-contents/DBFilesClient).

Upon downloading the repo, you'll have the patch-contents file available to you. Copy all the contents of the DBFilesClient folder to your AzerothCore Data directory. Feel free to make a backup of the DBCs you'll be replacing, as backups never hurt.

### 3) Compile and install AzerothCore.

### 4) Move the [patch-contents](https://github.com/benjymansy123/mod-worgoblin/tree/master/patch-contents) folder to your Data folder in your WoW client and rename the folder to `patch-A.MPQ`.

### 5) Remove signature checks from your WoW executable.
I recommend Windows users to use [this patcher](https://model-changing.net/index.php?app=downloads&module=downloads&controller=view&id=314&tab=details) created by kebabstorm to patch your Wow.exe file. I recommend macOS users to download this [pre-patched .app file](https://github.com/benjymansy123/custom-race-ac-12_6_21/releases/download/sig-check/WoW.app.zip), since no easy patcher exists for macOS.

Because WoW uses signature checks so as to not allow Interface files to be modified, we need to remove those checks in order to be able to use custom races. This is accomplished using the patcher included in the repo. Move the patcher into your WoW client directory and run it. Again, feel free to make a backup of your original executable.

And with that, you are all done!

## Credits

* mthsena for creating a repository for the [original script](https://github.com/mthsena/trinitycore_scripts/tree/master/scripts/CustomRaces) for TrinityCore.
* [Helias](https://github.com/Helias) for mentioning the script and adapting the script to AzerothCore.
* [yuan2105](https://github.com/yuanf225) for racing me to get these working and helping me out on multiple occasions.
* [Tanados](https://github.com/helldragonpz) for adapting the HD patch to work with the module.
* Trimitor#3873 for creating the HD patch.
* Various users on various Discords for helping me out on a slew of issues with almost everything. (ragestriker#8037 and Mr.MA#0957 in particular)
