DROP TABLE IF EXISTS `zone_difficulty_info`;
CREATE TABLE `zone_difficulty_info` (
    `MapID` INT NOT NULL DEFAULT 0,
    `PhaseMask` INT NOT NULL DEFAULT 0,
    `HealingNerfValue` FLOAT NOT NULL DEFAULT 1,
	`AbsorbNerfValue` FLOAT NOT NULL DEFAULT 1,
	`MeleeDmgBuffValue` FLOAT NOT NULL DEFAULT 1,
	`SpellDmgBuffValue` FLOAT NOT NULL DEFAULT 1,
    `Enabled` TINYINT DEFAULT 1,
	`Comment` TEXT,
	PRIMARY KEY (`MapID`, `PhaseMask`, `Enabled`)
);
