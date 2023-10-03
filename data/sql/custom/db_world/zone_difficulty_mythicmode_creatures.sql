DELETE FROM `creature_template` WHERE `entry` IN (1128001,1128002);

DELETE FROM `creature` WHERE `guid` IN
(303000,303001,303002,303003,303004,303005,303006,303007,303008,303009,303010,303011,303012,303013,303014,303015,303016,303017,303018);

DELETE FROM `npc_text` WHERE `ID` IN (91301,91302,91303,91304,91305,91306,91307,91308,91309,91310,91311);

INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `DamageModifier`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(1128001, 0, 0, 0, 0, 0, 10008, 0, 0, 0, 'Chromie', '', 0, 63, 63, 0, 35, 1, 1, 1.14286, 1, 0, 0, 1, 2000, 2000, 1, 1, 1, 33536, 2048, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1.35, 1, 1, 0, 0, 1, 0, 0, 2, 'mod_zone_difficulty_dungeonmaster', 0),
(1128002, 0, 0, 0, 0, 0, 27568, 0, 0, 0, 'Chromie', '', 0, 63, 63, 0, 35, 1, 1, 1.14286, 1, 0, 0, 1, 2000, 2000, 1, 1, 1, 33536, 2048, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1.35, 1, 1, 0, 0, 1, 0, 0, 2, 'mod_zone_difficulty_rewardnpc', 0);

INSERT INTO `creature` (`guid`, `id1`, `id2`, `id3`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`, `ScriptName`, `VerifiedBuild`) VALUES
 -- Reward NPC
(303000, 1128002, 0, 0, 530, 3703, 3703, 1, 1, 0, -1809.08, 5294.03, -12.4, 1.98, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_rewardnpc', 0),
  -- Gruul's Lair
(303001, 1128001, 0, 0, 565, 3923, 3923, 1, 1, 0, 76.64, 55.82, -5.38, 3.29, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- Magtheridon's Lair
(303002, 1128001, 0, 0, 544, 3836, 3836, 1, 1, 0, 232.00, 9.90, 68.03, 3.17, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- Blood Furnace
(303003, 1128001, 0, 0, 542, 3713, 3713, 2, 1024, 0, -9.9, 7.28, -44.6, 0.45, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- Hellfire Ramparts
(303004, 1128001, 0, 0, 543, 3562, 3562, 2, 1024, 0, -1362.3, 1650.80, 68.42, 5.77, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- Slave Pens
(303005, 1128001, 0, 0, 547, 3717, 3717, 2, 1024, 0, 135.1, -121.3, -1.56, 2.86, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- The Underbog
(303006, 1128001, 0, 0, 546, 3716, 3716, 2, 1024, 0, 11.9, -29.06, -2.75, 0.99, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- Mana-Tombs
(303007, 1128001, 0, 0, 557, 3792, 3792, 2, 1024, 0, -3.57, -6.31, -0.95, 1.59, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- Auchenai Crypts
(303008, 1128001, 0, 0, 558, 3790, 3790, 2, 1024, 0, -17.65, 7.91, -0.12, 4.72, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- The Escape From Durnholde
(303009, 1128001, 0, 0, 560, 2367, 2367, 2, 1024, 0, 2705.16, 1320.52, 14.06, 5.82, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- Sethekk Halls
(303010, 1128001, 0, 0, 556, 3791, 3791, 2, 1024, 0, 1.56, 7.91, 0.01, 4.88, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- Magisters' Terrace
(303011, 1128001, 0, 0, 585, 4131, 4131, 2, 1024, 0, 6.79, 9.90, -2.81, 4.74, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- Shadow Labyrinth
(303012, 1128001, 0, 0, 555, 3789, 3789, 2, 1024, 0, -3.26, -8.02, -1.12, 1.46, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- Shattered Halls
(303013, 1128001, 0, 0, 540, 3714, 3714, 2, 1024, 0, -35.76, -16.70, -13.87, 2.93, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- The Arcatraz
(303014, 1128001, 0, 0, 552, 3848, 3848, 2, 1024, 0, 20.11, 6.33, -0.16, 4.56, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- The Black Morass
(303015, 1128001, 0, 0, 269, 2366, 2366, 2, 1024, 0, -1483.39, 7069.76, 32.80, 3.77, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- The Botanica
(303016, 1128001, 0, 0, 553, 3847, 3847, 2, 1024, 0, 26.34, -23.81, -1.06, 0.58, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- The Mechanar
(303017, 1128001, 0, 0, 554, 3849, 3849, 2, 1024, 0, -25.95, 14.52, -1.81, 5.08, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0),
 -- The Steamvault
(303018, 1128001, 0, 0, 545, 3715, 3715, 2, 1024, 0, 9.83, 9.14, -3.86, 3.32, 1000000, 0, 0, 0, 0, 0, 0, 0, 0, 'mod_zone_difficulty_dungeonmaster', 0);

INSERT INTO `npc_text` (`ID`, `text0_0`, `BroadcastTextID0`, `lang0`, `Probability0`, `em0_0`, `em0_1`, `em0_2`, `em0_3`, `em0_4`, `em0_5`, `BroadcastTextID1`, `lang1`, `Probability1`, `em1_0`, `em1_1`, `em1_2`, `em1_3`, `em1_4`, `em1_5`, `BroadcastTextID2`, `lang2`, `Probability2`, `em2_0`, `em2_1`, `em2_2`, `em2_3`, `em2_4`, `em2_5`, `BroadcastTextID3`, `lang3`, `Probability3`, `em3_0`, `em3_1`, `em3_2`, `em3_3`, `em3_4`, `em3_5`, `BroadcastTextID4`, `lang4`, `Probability4`, `em4_0`, `em4_1`, `em4_2`, `em4_3`, `em4_4`, `em4_5`, `BroadcastTextID5`, `lang5`, `Probability5`, `em5_0`, `em5_1`, `em5_2`, `em5_3`, `em5_4`, `em5_5`, `BroadcastTextID6`, `lang6`, `Probability6`, `em6_0`, `em6_1`, `em6_2`, `em6_3`, `em6_4`, `em6_5`, `BroadcastTextID7`, `lang7`, `Probability7`, `em7_0`, `em7_1`, `em7_2`, `em7_3`, `em7_4`, `em7_5`, `VerifiedBuild`) VALUES
(91301, 'Hello, $n. How strong do you think you are? I am offering 2 different versions to witness the events of the past. If you don\'t tell me otherwise, i will show you the cinematic one.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(91302, 'Hello, $n. The leader of your party will have the final say about what you\'re going for.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(91303, 'Hello, $n. So you and your mates are after a challenge? I appreciate that. I will show you the real experience. Better be prepared!', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(91304, 'Are you sure you wish to leave the historic path? There will be no turning back for this time.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(91305, 'My pleasure to meet you, $n. I am offering rewards for the strongest and the toughest of all time-travelers. Did you stabilise the timeline enough to receive one of my rewards? What course did you run?', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(91306, 'Please pick a category for the item you desire?', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(91307, 'Which item would you like to obtain?', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(91308, 'Are you sure this is the right item? I don\'t do refunds!', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(91309, 'I can grant that wish! Please check your mail and enjoy!', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(91310, 'I\'m afraid that is beyond my capabilities for now. You should finish fixing the timeline before you request this.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(91311, 'Of course! Listen well, $n.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
