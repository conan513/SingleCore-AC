-- ************************************************************************
-- Possible values for `Target`:
SET @TARGET_NONE = 0;                       -- For spells without target.
SET @TARGET_SELF = 1;                       -- Self cast.
SET @TARGET_VICTIM = 2;                     -- Our current target. (ie: highest aggro)
SET @TARGET_HOSTILE_AGGRO_FROM_TOP= 3;      -- Position (TargetArg2) in the threat list counted from top. TargetArg = max/min range. Fallback to GetVictim().
SET @TARGET_HOSTILE_AGGRO_FROM_BOTTOM = 4;  -- Position (TargetArg2) in the threat list counted from bottom. TargetArg = max/min range. Fallback to GetVictim().
SET @TARGET_HOSTILE_RANDOM = 5;             -- Just any random player on our threat list. TargetArg = max/min range.
SET @TARGET_HOSTILE_RANDOM_NOT_TOP = 6;     -- Just any random player on our threat list except the current target. TargetArg = max/min range.
SET @TARGET_PLAYER_DISTANCE = 18;           -- All players in range. TargetArg = max/min range.
-- ************************************************************************

DELETE FROM `zone_difficulty_mythicmode_ai` WHERE CreatureEntry IN (21136, 21137, 21138, 21139, 17879, 17881, 18701);
INSERT INTO `zone_difficulty_mythicmode_ai` (`CreatureEntry`, `Chance`, `Spell`, `Spellbp0`, `Spellbp1`, `Spellbp2`, `Target`, `TargetArg`, `TargetArg2`, `Delay`, `Cooldown`, `Repetitions`, `Enabled`, `TriggeredCast`, `Comment`) VALUES
-- Black Morrass Trash
(21136, 30, 43242, 0, 0, 0, @TARGET_SELF, 0, 0, 2000, 0, 1, 1, 1, 'Infinite Chronomancer, Black Morrass. Haste (Movement and cast) on self after 2 seconds once. 30% Chance.'),
(21136, 15, 51758, 0, 0, 0, @TARGET_SELF, 0, 0, 2000, 20000, 0, 1, 1, 'Infinite Chronomancer, Black Morrass. Fire Reflection on self after 2s every 20s. 15% Chance'),
(21136, 15, 51763, 0, 0, 0, @TARGET_SELF, 0, 0, 4000, 20000, 0, 1, 1, 'Infinite Chronomancer, Black Morrass. Frost Reflection on self after 4s every 20s. 15% Chance'),
(21136, 15, 51764, 0, 0, 0, @TARGET_SELF, 0, 0, 6000, 20000, 0, 1, 1, 'Infinite Chronomancer, Black Morrass. Shadow Reflection on self after 6s every 20s. 15% Chance'),
(21136, 15, 51766, 0, 0, 0, @TARGET_SELF, 0, 0, 8000, 20000, 0, 1, 1, 'Infinite Chronomancer, Black Morrass. Arcane Reflection on self after 8s every 20s. 15% Chance'),
(21137, 30, 43242, 0, 0, 0, @TARGET_SELF, 0, 0, 2000, 0, 1, 1, 1, 'Infinite Assassin, Black Morrass. Haste (Movement and cast) on self after 2 seconds once. 30% chance.'),
(21137, 15, 51758, 0, 0, 0, @TARGET_SELF, 0, 0, 2000, 20000, 0, 1, 1, 'Infinite Assassin, Black Morrass. Fire Reflection on self after 2s every 20s. 15% Chance'),
(21137, 15, 51763, 0, 0, 0, @TARGET_SELF, 0, 0, 4000, 20000, 0, 1, 1, 'Infinite Assassin, Black Morrass. Frost Reflection on self after 4s every 20s. 15% Chance'),
(21137, 15, 51764, 0, 0, 0, @TARGET_SELF, 0, 0, 6000, 20000, 0, 1, 1, 'Infinite Assassin, Black Morrass. Shadow Reflection on self after 6s every 20s. 15% Chance'),
(21137, 15, 51766, 0, 0, 0, @TARGET_SELF, 0, 0, 8000, 20000, 0, 1, 1, 'Infinite Assassin, Black Morrass. Arcane Reflection on self after 8s every 20s. 15% Chance'),
(21138, 30, 43242, 0, 0, 0, @TARGET_SELF, 0, 0, 2000, 0, 1, 1, 1, 'Infinite Executioner, Black Morrass. Haste (Movement and cast) on self after 2 seconds once. 30% chance.'),
(21138, 30, 40545, 0, 0, 0, @TARGET_SELF, 0, 0, 1000, 20000, 0, 0, 1, 'Infinite Executioner, Black Morrass. Unholy Growth on self after 1s every 20s. 10s cast time. 30% chance.'),
(21139, 30, 43242, 0, 0, 0, @TARGET_SELF, 0, 0, 2000, 0, 1, 1, 1, 'Infinite Vanquisher, Black Morrass. Haste (Movement and cast) on self after 2 seconds once. 30% chance.'),
(21139, 30, 40545, 0, 0, 0, @TARGET_SELF, 0, 0, 1000, 20000, 0, 0, 1, 'Infinite Vanquisher, Black Morrass. Unholy Growth on self after 1s every 20s. 10s cast time. 30% chance.'),
(21818, 20, 40545, 0, 0, 0, @TARGET_SELF, 0, 0, 1000, 20000, 0, 0, 1, 'Infinite Whelp, Black Morrass. Unholy Growth on self after 1s every 20s. 10s cast time. 20% chance.'),
-- Black Morrass Bosses
(17879, 100, 55948, 0, 0, 0, @TARGET_SELF, 0, 0, 15000, 10000, 0, 1, 1, 'Chrono Lord Deja, Black Morrass. Grow on self after 15s every 10s.'),
(17881, 100, 32663, 0, 0, 0, @TARGET_SELF, 0, 0, 15000, 8000, 0, 1, 1, 'Aeonus, Black Morrass. Summon Dark Vortex after 15s every 8s.'),
(18701, 100, 7, 0, 0, 0, @TARGET_SELF, 0, 2, 60000, 0, 1, 1, 1, 'Dark Vortex, summoned by Aeonus Black, Morrass (Line above). Kill self after 60s.'),
(18701, 30, 9906, 0, 0, 0, @TARGET_SELF, 0, 2, 3000, 10000, 0, 1, 1, 'Dark Vortex, summoned by Aeonus Black, Morrass (Line above). Reflect for 5s every 10s after 3s. 30% Chance.'),
(18701, 30, 63240, 0, 0, 0, @TARGET_SELF, 0, 2, 1000, 20000, 0, 1, 1, 'Dark Vorte, summoned by Aeonus Black, Morrass (Line above). Thorns for 10s every 20s after 1s. 30% Chance.');
