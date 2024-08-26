-- create the table we use to store market data
CREATE TABLE IF NOT EXISTS `mod_auctionator_market_price` (
    `entry` INT,
    `average_price` INT, 
    `buyout` INT,
    `bid` INT,
    `count` INT,
    `scan_datetime` DATETIME,
    PRIMARY KEY (`entry`),
    Index(`entry`, `scan_datetime`)
);
