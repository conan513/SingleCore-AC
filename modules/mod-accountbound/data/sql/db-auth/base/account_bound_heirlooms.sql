CREATE TABLE IF NOT EXISTS `account_bound_heirlooms` (
    `account_id` INT(10) UNSIGNED NOT NULL,
    `item_id` MEDIUMINT(8) UNSIGNED NOT NULL,
    PRIMARY KEY (`account_id`, `item_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
