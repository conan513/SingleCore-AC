# Zone Difficulty Module

[English](README.md) | [Español](README_ES.md)


## What is the purpose of this module?
This module provides a way to nerf or debuff players in order to increase the difficulty of certain dungeons or balance battlegrounds/duels.

## How to use this module?
This module reads data from the `zone_difficulty_info` and `zone_difficulty_spelloverrides` table.
`_info` serves to apply nerfs/debuffs per map and per phase. For example, you can nerf healing
by 90% and absorb-effects by 80% while increasing the melee damage players suffer by 50% and reducing the
spell damage players suffer by 30%. That is only inside the Temple of Ahn'Qiraj dungeon (MapId 531)
for all phases (0 = Skip PhaseMask check, 6 = phases 2 and 4) with the following query:

```sql
INSERT INTO `zone_difficulty_info` (`MapId`, `PhaseMask`, `HealingNerfValue`, `AbsorbNerfValue`, `MeleeDmgBuffValue`, `SpellDmgBuffValue`, `Enabled`, `Comment`) VALUES
(531, 0, '0.10', '0.20', '1.50', '0.70', 1, 'AQ40 Healing 50% / Absorb 50% Nerf / 50% physical & 30% spell damage buff');
```

The heal-output will be multiplied by the value you input in `HealingNerfValue` as it follows:

```cpp
output = output * HealingNerfValue
```

Using `MapId` 2147483647 will be used for all targets in duels while they're in the zone hardcoded as `DUEL_AREA` (default 2402: Forbidding Sea, Wetlands).
PhaseMask must be 0 for Duels.

You can also prevent certain spells from being affected at all. See `zone_difficulty_spelloverrides.sql` for examples.

## Changing values

You can edit the `zone_difficulty_info_content.sql` file to apply changes. They will be applied by the autoupdater the next time you restart your server.
Alternatively, you may edit the `zone_difficulty_info` table in your `world` database and use the `reload config` command to reload the values.

# Mythicmode

## How does it work?

For the Mythicmode, there is an NPC (id 1128001) spawned at the beginning of raids and the daily heroic dungeon,
determined by the quest given from Wind Trader Zhareem in Shattrath. (Addition of WotlK heroics outstanding).
The leader of the party can chose to activate the Mythicmode by talking to the NPC inside the heroic
dungeon / raid. Single players can not use the Mythicmode. In Heroic dungeons, the NPC will despawn
after 60 seconds. The party needs to decide, if they want to play the Mythicmode before this happens.
The last boss of the heroic dungeon will award one score to every player in the heroic dungeon.

In Raids, the NPC will stay indefinitely. As soon as one raidboss is defeated, the leader can still turn the
Mythicmode off, but no longer on once it is deactivated. Every defeated raidboss grants one score in the
respective category.

## Config

`ModZoneDifficulty.Hardmode.HpModifier = 2.0` determines the default creature hp modifier for Mythicmode.

## Tables
`zone_difficulty_instance_saves` creates the tables to store the Mythicmode status and the scores.
No action required.

`zone_difficulty_creatureoverrides` contains overrides for creature hp values. This is used to vary
certain creatures health from the default value in the conf file.

`zone_difficulty_mythicmode_creatures` contains gossips and creature positions for the NPCs inside dungeons.
Requires spawning additional NPCs, if more heroic dungeons / raids should be added.

In heroic dungeons, the default spawn mask should be 1024. The script will change the NPCs phasemask to
1, if the heroic dungeon is the current daily. In raids, the default spawn mask should be 1.

`zone_difficulty_instance_data` contains the encounter IDs which should give scores
for the Mythicmode. The encounter IDs are taken from the `creature_template` table.
`OverrideGO` is reserved for future use. `InstanceType` refers to the content level, see the top
of the file for a list.

`zone_difficulty_mythicmode_rewards` holds the items which can be chosen as a reward for the Mythicmode.
Tho gossips for the reward npc (id 1128002) will automatically expand, depending on the kind and
amount of rows in the table. `ContentType` refers to the content level, `ItemType` refers to the item
category. See the top of the file for a list for both. By adding an enchant id and a slot to the item,
a custom enchant can be applied.

## Authors

- [Nyeriah](https://github.com/Nyeriah)
- [Honey](https://github.com/55Honey)
