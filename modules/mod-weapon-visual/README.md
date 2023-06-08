# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore
## mod-weapon-visual
### This is a module for [AzerothCore](http://www.azerothcore.org)
- Latest build status with azerothcore: [![Build Status](https://github.com/azerothcore/mod-weapon-visual/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-weapon-visual)

## Important notes

If you use an old version of this module please update the table structure using this SQL statement:
```sql
ALTER TABLE `custom_item_enchant_visuals`
CHANGE `iguid` `item_guid` int(10) unsigned NOT NULL COMMENT 'item DB guid' FIRST,
CHANGE `display` `enchant_visual_id` int(10) unsigned NOT NULL COMMENT 'enchantID' AFTER `item_guid`,
DROP `PlayerName`,
RENAME TO `mod_weapon_visual_effect`,
COMMENT='Stores the enchant IDs for the visuals of mod-weapon-visual';
```

## Description
- Character services: Adding Visual style for Weapons

### How to install
1. Simply place the module under the `modules` folder of your AzerothCore source folder.
2. Re-run cmake and launch a clean build of AzerothCore
3. Import `sql/world/base/creature_template.sql` into your world DB.
4. Import `sql/characters/base/mod_weapon_visual_effect.sql` into your characters DB.
5. Log in game, spawn Npc ID 900000, Enjoy.

### Usage
- Enable this module and talk with the assigned creature :D

### Showcase / Video 
- [Video Here!](https://youtu.be/Sat9KWvsPwQ)


## Credits
* [Poszer](https://github.com/Poszer): (Author of the module): 
* [Talamortis](https://github.com/Talamortis):  (Database implementation, code review & General support):
* [Micrah/Milestorme](https://github.com/milestorme): (Thanks for the help with the Module creation & General support): 

AzerothCore: [repository](https://github.com/azerothcore) - [website](http://azerothcore.org/) - [discord chat community](https://discord.gg/PaqQRkd)
