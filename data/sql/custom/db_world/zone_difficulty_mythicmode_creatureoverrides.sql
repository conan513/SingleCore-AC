DROP TABLE IF EXISTS `zone_difficulty_mythicmode_creatureoverrides`;
CREATE TABLE `zone_difficulty_mythicmode_creatureoverrides` (
    `CreatureEntry` INT NOT NULL DEFAULT 0,
    `HPModifier` FLOAT NOT NULL DEFAULT 1,
    `Enabled` TINYINT DEFAULT 1,
	`Comment` TEXT
);

DELETE FROM `zone_difficulty_mythicmode_creatureoverrides`;
INSERT INTO `zone_difficulty_mythicmode_creatureoverrides` (`CreatureEntry`, `HPModifier`, `Enabled`, `Comment`) VALUES
(1128001, 1, 1, 'Chromie NPC - prevent changing hp.'),
(18831, 2.5, 1, 'Maulgar, Gruul\'s Lair HPx2.5'),
(19044, 2.8, 1, 'Gruul, Gruul\'s Lair HPx2.8'),
(17257, 2.5, 1, 'Magtheridon, Magtheridon\'s Lair HPx2.5'),
(17881, 3.2, 1, 'Aeonus, Black Morass HPx3.2');
