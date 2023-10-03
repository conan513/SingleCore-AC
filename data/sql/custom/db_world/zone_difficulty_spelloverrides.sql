DROP TABLE IF EXISTS `zone_difficulty_spelloverrides`;
CREATE TABLE `zone_difficulty_spelloverrides` (
    `SpellID` INT NOT NULL DEFAULT 0,
    `MapId` INT NOT NULL DEFAULT 0,            -- ID of the map for this line to be relevant. 0 for all maps
    `NerfValue` FLOAT NOT NULL DEFAULT 1,
    `Enabled` TINYINT DEFAULT 1,               -- Modes for this multiplier to be enabled. 1 = normal 64 = mythic
	`Comment` TEXT
);

DELETE FROM `zone_difficulty_spelloverrides` WHERE `SpellID` IN (53652, 53654, 52752, 47753, 18562, 25516, 36240);
INSERT INTO `zone_difficulty_spelloverrides` (`SpellID`, `MapId`, `NerfValue`, `Enabled`, `Comment`) VALUES
(53652, 0, 1, 1, 'Beacon of Light - Skip nerf.'),
(53654, 0, 1, 1, 'Beacon of Light 2 (Flash of Light, Holy Shock) - Skip nerf.'),
(52752, 0, 1, 1, 'Ancestral Awakening - Skip nerf.'),
(47753, 0, 1, 1, 'Divine Aegis - Skip nerf.'),
(18562, 0, 1, 1, 'Swiftmend - Skip nerf.'),
(25516, 0, 1, 1, 'AQ20 Andorov Healing - Skip nerf.'),
(36240, 0, 1, 1, 'Gruul Cave In - Skip buff.');
