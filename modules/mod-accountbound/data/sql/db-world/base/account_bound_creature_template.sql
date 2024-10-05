SET
@Entry    := 7500000,
@Model    := 25609,
@Name     := "Edward Findle",
@Title    := "Heirloom Hoarder",
@Icon     := "Speak",
@MinLevel := 25,
@MaxLevel := 25,
@Faction  := 35,
@NPCFlag  := 129,
@Rank     := 0,
@Type     := 7,
@Script   := "npc_heirloom_hoarder";

-- Creature template
DELETE FROM `creature_template` WHERE `entry`=@Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `minlevel`, `maxlevel`, `faction`, `npcflag`, `unit_class`, `unit_flags`, `type`, `ScriptName`) VALUES
(@Entry, @Name, @Title, @Icon, @MinLevel, @MaxLevel, @Faction, @NPCFlag, 1, 2, @Type, @Script);

-- Creature Template Model
DELETE FROM `creature_template_model` WHERE `CreatureID`=@Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`) VALUES
(@Entry, 0, @Model, 1, 1);
