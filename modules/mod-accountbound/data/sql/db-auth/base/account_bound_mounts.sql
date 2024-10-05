CREATE TABLE IF NOT EXISTS `account_bound_mounts` (
    `account_id` INT(10) UNSIGNED NOT NULL,
    `spell_id` MEDIUMINT(8) UNSIGNED NOT NULL,
    `allowable_race` INT(11) NOT NULL,
    `allowable_class` INT(11) NOT NULL,
    `required_level` TINYINT(3) UNSIGNED NOT NULL,
    `required_skill` SMALLINT(3) UNSIGNED NOT NULL,
    `required_skill_rank` SMALLINT(3) UNSIGNED NOT NULL,
    PRIMARY KEY (`account_id`,`spell_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
