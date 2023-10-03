DROP TABLE IF EXISTS `mod_classtrial_characters`;
CREATE TABLE IF NOT EXISTS `mod_classtrial_characters` (
	`GUID` INT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier for characters using a class trial',
	PRIMARY KEY (`GUID`)
) COLLATE='utf8mb4_general_ci';
