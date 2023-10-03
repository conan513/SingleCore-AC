CREATE TABLE IF NOT EXISTS `zone_difficulty_instance_saves`(
    `InstanceID` INT NOT NULL DEFAULT 0,
    `MythicmodeOn` TINYINT NOT NULL DEFAULT 0,
    `MythicmodePossible` TINYINT NOT NULL DEFAULT 1,
    PRIMARY KEY (`InstanceID`)
);

CREATE TABLE IF NOT EXISTS `zone_difficulty_mythicmode_score`(
    `GUID` INT NOT NULL DEFAULT 0,
    `Type` TINYINT NOT NULL DEFAULT 0,
    `Score` INT NOT NULL DEFAULT 0,
    PRIMARY KEY (`GUID`, `Type`)
);

CREATE TABLE IF NOT EXISTS `zone_difficulty_encounter_logs`(
    `InstanceId` INT NOT NULL DEFAULT 0,
    `TimestampStart` INT NOT NULL DEFAULT 0,
    `TimestampEnd` INT NOT NULL DEFAULT 0,
    `Map` INT NOT NULL DEFAULT 0,
    `BossId` INT NOT NULL DEFAULT 0,
    `PlayerGuid` INT NOT NULL DEFAULT 0,
    `Mode` INT NOT NULL DEFAULT 0,
    PRIMARY KEY (`InstanceId`, `TimestampStart`, `PlayerGuid`)
);
