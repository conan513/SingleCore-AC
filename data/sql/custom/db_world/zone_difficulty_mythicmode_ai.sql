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

DROP TABLE IF EXISTS `zone_difficulty_mythicmode_ai`;
CREATE TABLE `zone_difficulty_mythicmode_ai` (
    `CreatureEntry` INT NOT NULL DEFAULT 0,
    `Chance` TINYINT NOT NULL DEFAULT 100,          -- 0-100% chance for the creature to gain this spell
    `Spell` INT NOT NULL,                           -- spell id
    `Spellbp0` INT NOT NULL DEFAULT 0,              -- custom spell value bp0
    `Spellbp1` INT NOT NULL DEFAULT 0,              -- custom spell value bp1
    `Spellbp2` INT NOT NULL DEFAULT 0,              -- custom spell value bp2
    `Target` TINYINT NOT NULL DEFAULT 1,            -- see above
    `TargetArg` INT NOT NULL DEFAULT 0,             -- optional argument for the target. Max/min range
    `TargetArg2` INT NOT NULL DEFAULT 0,            -- optional argument for the target. Counter in the threat list for position
    `Delay` INT NOT NULL DEFAULT 1,                 -- time in ms before first cast
    `Cooldown` INT NOT NULL DEFAULT 1,              -- time in ms between casts
    `Repetitions` TINYINT NOT NULL DEFAULT 0,       -- 0 = forever, 1 = just once. Room for future counters.
    `Enabled` TINYINT DEFAULT 1,                    -- 0 = disabled, 1 = enabled
	`TriggeredCast` TINYINT DEFAULT 1,              -- 0 = Not triggered, 1 = Triggered spell cast (no cast time, etc.)
	`Comment` TEXT
);
