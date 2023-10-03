Set @NpcName = "Eron Glowpride",
    @NpcSubname = "Weapon Glow Specialist",
    @NpcEntry = 900000,
    @NpcDisplayID = 31833,
    @NpcLevel = 80;

DELETE FROM `creature_template` WHERE `entry` = @NpcEntry;
INSERT INTO `creature_template` ( `entry`, `modelid1`, `name`, `subname`, `minlevel`, `maxlevel`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(@NpcEntry, @NpcDisplayID, @NpcName, @NpcSubname, @NpcLevel, @NpcLevel, 35, 1, 1, 1.14286, 1, 1, 0, 2000, 2000, 2, 0, 2048, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 50, 50, 1, 0, 0, 1, 0, 0, 'npc_visualweapon', 12340);
