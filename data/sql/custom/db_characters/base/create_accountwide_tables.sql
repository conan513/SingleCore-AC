-- Ensure we are using the characters database
USE `acore_characters`;

-- Create the accountwide_achievements table if it doesn't already exist
CREATE TABLE IF NOT EXISTS `accountwide_achievements` (
    `accountId` INT UNSIGNED NOT NULL,
    `achievementId` INT UNSIGNED NOT NULL,
    PRIMARY KEY (`accountId`, `achievementId`)
);

-- Create the accountwide_currency table if it doesn't already exist
CREATE TABLE IF NOT EXISTS `accountwide_currency` (
    `accountId` INT UNSIGNED NOT NULL,
    `currencyId` INT UNSIGNED NOT NULL,
    `count` INT NOT NULL,
    PRIMARY KEY (`accountId`, `currencyId`)
);

-- Create the accountwide_money table if it doesn't already exist
CREATE TABLE IF NOT EXISTS `accountwide_money` (
    `accountId` INT UNSIGNED NOT NULL PRIMARY KEY,
    `money` BIGINT UNSIGNED NOT NULL DEFAULT 0
);

-- Create the accountwide_mounts table if it doesn't already exist
CREATE TABLE IF NOT EXISTS `accountwide_mounts` (
    `accountId` INT UNSIGNED NOT NULL,
    `mountSpellId` INT UNSIGNED NOT NULL,
    PRIMARY KEY (`accountId`, `mountSpellId`)
);

-- Create the accountwide_pets table if it doesn't already exist
CREATE TABLE IF NOT EXISTS `accountwide_pets` (
    `accountId` INT UNSIGNED NOT NULL,
    `petSpellId` INT UNSIGNED NOT NULL,
    PRIMARY KEY (`accountId`, `petSpellId`)
);

-- This table is now obsolete, so remove it for players who were using the table prior to this change
DROP TABLE IF EXISTS `accountwide_reputation`;