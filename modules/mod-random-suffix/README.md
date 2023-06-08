# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore
## Mod Random Suffix

<!-- TODO: Do up badge again -->
<!-- - Latest build status with azerothcore: [![Build Status](https://github.com/azerothcore/mod-random-enchants/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-random-enchants) -->

<b>Make sure to support the main project:
https://github.com/azerothcore/azerothcore-wotlk/</b>

This module adds in more random custom suffixes and includes a system where all items can roll these new custom random suffixes. This module began as a mere fork of the original [mod-random-enchants](https://github.com/azerothcore/mod-random-enchants) with more hardcoded enchants but grew to one where random custom suffixes with actual stat calculations being shown on the clientside.

__Below are some pictures of what this module does__

| Item with original stats | Item in-game w/ a rolled suffix |
| ------------------------ | ------------------------------- |
| ![](docs/images/original_item_stats.png) |  ![](docs/images/item_stats_w_suffix.png)     |


__Suffix correctly rendering in chat__

![](docs/images/valid_item_linking_in_chat.png)

__Meaningful stat comparisons (i.e. showing correct diffs when shift clicking an item)__

![](docs/images/meaningful_stat_comparisons.png)

This module works by tapping on several patches for both the server-side (Azerothcore) and client-side (WoW game client) to get `ItemRandomSuffix.dbc` to show meaningful stats shown when looking at items items in-game (i.e. proper stat calculation via allocation points), as well as actual suffix names on items.

## Important Note
This module also makes use of client patching and modification. It is recommended to use a [clean unmodified enUS WoW client](https://www.chromiecraft.com/en/downloads/). A patcher for the WoW client is provided under the `patcher-WoWClient` folder taken from this [forum link](https://model-changing.net/index.php?app=downloads&module=downloads&controller=view&id=314&tab=details) which will remove signature checks, allowing for us to include custom suffixes with names and correct stat calculations on the WoW client side. Do keep a backup of the unmodified client just in case.

## Installation
1. Apply the `acore-modrandomsuffix.patch` to your azerothcore source code. There are multiple ways to achieve this. You can do it using git via this command:
```
git apply --ignore-space-change --ignore-whitespace modules/mod-random-suffix/acore-modrandomsuffix.patch
```
2. Compile, install and run Azerothcore. No replacement of DBC files on the *server side* should be required as the azerothcore DB importer should automagically pick up the custom suffixes to be imported via the `data/sql/db-world` folder.
3. Copy the whole `patch-Z.MPQ` folder into your WoW client `Data` folder.
4. Remove the signature checks via running the patcher inside `patcher-WoWClient`. Copy the `exe` file into the root of your WoW client folder (The folder with `WoW.exe`). Make a backup of `WoW.exe` just in case, doublecheck the checksum of your `WoW.exe` via this link https://github.com/anzz1/wow-client-checksums.

## Uninstallation
0. (Optional) If you want to be safe, ensure that items that have custom random suffixes are all removed. I have not tested out uninstallation but the core should merely just flag out or straight up not process the random suffix / enchanted slots by default.
1. Remove the server-side patch on azerothcore source code. There are multiple ways to achieve this, you can probably do it using git via this command:
```
git apply -R modules/mod-random-suffix/acore-modrandomsuffix.patch
```
2. Re-run CMake, compile and re-install Azerothcore
3. Remove the whole `patch-Z.MPQ` folder from your WoW `Data` folder.
4. (Optional) Move the clean copy of your WoW.exe that you've backed up to its original name.

# Advanced usage

This following module has another component which is a golang codebase that generates the SQL and DBC files required for this module to function.

In the root directory of this module, the sample config file `generatesuffixes.conf.yaml` is used to pre-generate the following files in these directories:
- `data/sql/db-world/mod_acore_random_suffix.sql`
- `patch-Z.MPQ/DBFilesClient/ItemRandomSuffix.dbc`
- `patch-Z.MPQ/DBFilesClient/SpellItemEnchantment.dbc`

The pre-generated files will suffice in most cases, but users may change the names / allocation point tier thresholds and other configuration via this file, and then run the following command (Assuming you have a `go` installed)

```
go run ./golang/cmd/generatesuffixes/main.go
```

Ideally this should be done on a **CLEAN** azerothcore server and not applied once again after that. I make no assumptions of the possibility that nothing will go wrong if we try to change the generated suffixes partway through a server's lifetime.

# Credits
- That one guy that wrote the initial LUA script which 3ndos used to create the original module.
- [3ndos](https://github.com/3ndos) for creating the original module code for azerothcore of which the main azerothcore `mod-random-enchants` is forked from https://github.com/azerothcore/mod-random-enchants
- [The Azerothcore team](https://github.com/azerothcore/) for creating Azerothcore (https://github.com/azerothcore/azerothcore-wotlk). A project that has rekindled my love for WoW as well as game modding.
- The [WoWGaming project](https://wowgaming.github.io/about-us/) with its tools such as the [node-dbc-reader](https://github.com/wowgaming/node-dbc-reader), of which I studied intensely to get a better understanding on how to implement a DBC reader and appender for this project.
- [heyitsbench](https://github.com/heyitsbench), for their [mod-worgoblin](https://github.com/heyitsbench/mod-worgoblin) module giving me the idea and possibility of actually modding the client to get custom suffixes up.

# License
- The previous source code written by [3ndos](https://github.com/3ndos) at https://github.com/3ndos/RandomEnchantsModule and subsequently the forked azerothcore repo https://github.com/azerothcore/mod-random-enchants are both unlicensed so I decided to leave it as it is.
- The golang source code containing the suffix generator and a simple DBC reader/writer are released as MPL 2.0.
