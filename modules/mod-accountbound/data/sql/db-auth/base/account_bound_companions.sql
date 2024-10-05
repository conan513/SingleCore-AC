CREATE TABLE IF NOT EXISTS `account_bound_companions` (
    `account_id` INT(10) UNSIGNED NOT NULL,
    `spell_id` MEDIUMINT(8) UNSIGNED NOT NULL,
    `allowable_race` INT(11) UNSIGNED NOT NULL,
    PRIMARY KEY (`account_id`, `spell_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;