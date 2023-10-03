-- Content types:
-- TYPE_VANILLA = 1;
-- TYPE_RAID_MC = 2;
-- TYPE_RAID_ONY = 3;
-- TYPE_RAID_BWL = 4;
-- TYPE_RAID_ZG = 5;
-- TYPE_RAID_AQ20 = 6;
-- TYPE_RAID_AQ40 = 7;
-- TYPE_HEROIC_TBC = 8;
-- TYPE_RAID_T4 = 9;
-- TYPE_RAID_T5 = 10;
-- TYPE_RAID_T6 = 11;
-- TYPE_HEROIC_WOTLK = 12;
-- TYPE_RAID_T7 = 13;
-- TYPE_RAID_T8 = 14;
-- TYPE_RAID_T9 = 15;
-- TYPE_RAID_T10 = 16;

-- Item types:
-- Back, Finger, Trinket, Neck = 1
-- Cloth = 2
-- Leather = 3
-- Mail = 4
-- Plate = 5
-- Weapons, Holdables, Shields = 6

DROP TABLE IF EXISTS `zone_difficulty_mythicmode_rewards`;
CREATE TABLE `zone_difficulty_mythicmode_rewards` (
    `ContentType` INT NOT NULL DEFAULT 0,
    `ItemType` INT NOT NULL DEFAULT 0,
    `Entry` INT NOT NULL DEFAULT 0,
    `Price` INT NOT NULL DEFAULT 0,
    `Enchant` INT NOT NULL DEFAULT 0,
    `EnchantSlot` TINYINT NOT NULL DEFAULT 0,
    `Achievement` INT NOT NULL DEFAULT 0,        -- indicates a full-tier-clearance reward if negative
    `Enabled` TINYINT DEFAULT 0,
    `Comment` TEXT,
    PRIMARY KEY (`ContentType`, `Entry`, `Enchant`)
);

INSERT INTO `zone_difficulty_mythicmode_rewards` (`ContentType`, `ItemType`, `Entry`, `Price`, `Enchant`, `EnchantSlot`, `Achievement`, `Enabled`, `Comment`) VALUES
-- Full Tier Rewards:
(8, 0, 13584, 20, 0, 0, -1, 1, 'Mini Diablo for clearing all TBC heroics on Mythicmode'),
(9, 0, 13583, 20, 0, 0, -1, 1, 'Panda Cub for clearing all T4 raids on Mythicmode'),

-- TYPE_HEROIC_TBC = 8;
-- Back, Finger, Trinket, Neck = 1
(8, 1, 29347, 10, 211, 11, 668, 1, 'Talisman of the Breaker +7 SP'),
(8, 1, 29349, 10, 1588, 11, 669, 1, 'Adamantine Chain of the Unbroken +14 AP'),
(8, 1, 29352, 10, 211, 11, 671, 1, 'Cobalt Band of Tyrigosa +7 SP'),
(8, 1, 31919, 10, 1588, 11, 671, 1, 'Nexus-Prince\'s Ring of Balance +14 AP'),
(8, 1, 31920, 10, 1588, 11, 671, 1, 'Shaffar\'s Band of Brutality +14 AP'),
(8, 1, 31921, 10, 211, 11, 671, 1, 'Yor\'s Collapsing Band +7 SP'),
(8, 1, 31922, 10, 211, 11, 671, 1, 'Ring of Conflict Survival +7 SP'),
(8, 1, 31923, 10, 211, 11, 671, 1, 'Band of the Crystalline Void +7 SP'),
(8, 1, 31924, 10, 2730, 11, 671, 1, 'Yor\'s Revenge +8 Dodge'),
(8, 1, 32081, 10, 1588, 11, 670, 1, 'Eye of the Stalker +14 AP'),
(8, 1, 29354, 10, 211, 11, 672, 1, 'Light-Touched Stole of Altruism +7 SP'),
-- Cloth = 2
(8, 2, 29240, 10, 211, 11, 671, 1, 'Bands of Negation +7 SP'),
(8, 2, 29241, 10, 211, 11, 681, 1, 'Belt of Depravity +7 SP'),
(8, 2, 29242, 10, 211, 11, 669, 1, 'Boots of Blasphemy +7 SP'),
(8, 2, 29249, 10, 211, 11, 672, 1, 'Bands of the Benevolent +7 SP'),
(8, 2, 29250, 10, 211, 11, 673, 1, 'Cord of Sanctification +7 SP'),
(8, 2, 29251, 10, 211, 11, 679, 1, 'Boots of the Pious +7 SP'),
(8, 2, 29255, 10, 211, 11, 678, 1, 'Bands of Rarefied Magic +7 SP'),
(8, 2, 29257, 10, 211, 11, 672, 1, 'Sash of Arcane Visions +7 SP'),
(8, 2, 29258, 10, 211, 11, 680, 1, 'Boots of Ethereal Manipulation +7 SP'),
(8, 2, 30531, 10, 211, 11, 676, 1, 'Breeches of the Occultist +7 SP'),
(8, 2, 30532, 10, 211, 11, 675, 1, 'Kirin Tor Master\'s Trousers +7 SP'),
(8, 2, 30543, 10, 211, 11, 677, 1, 'Pontifex Kilt +7 SP'),
-- Leather = 3
(8, 3, 29246, 10, 1588, 11, 673, 1, 'Nightfall Wristguards +14 AP'),
(8, 3, 29247, 10, 1588, 11, 676, 1, 'Girdle of the Deathdealer +14 AP'),
(8, 3, 29248, 10, 1588, 11, 681, 1, 'Shadowstep Striders +14 AP'),
(8, 3, 29263, 10, 1588, 11, 678, 1, 'Forestheart Bracers +14 AP'),
(8, 3, 29264, 10, 1588, 11, 667, 1, 'Tree-Mender\'s Belt +14 AP'),
(8, 3, 29265, 10, 1588, 11, 670, 1, 'Barkchip Boots +14 AP'),
(8, 3, 29357, 10, 1588, 11, 675, 1, 'Master Thief\'s Gloves +14 AP'),
(8, 3, 30535, 10, 1588, 11, 671, 1, 'Forestwalker Kilt +14 AP'),
(8, 3, 30538, 10, 1588, 11, 669, 1, 'Midnight Legguards +14 AP'),
(8, 3, 32080, 10, 1588, 11, 668, 1, 'Mantle of Shadowy Embrace +14 AP'),
-- Mail = 4
(8, 4, 29243, 10, 211, 11, 677, 1, 'Wave-Fury Vambraces +7 SP'),
(8, 4, 29244, 10, 211, 11, 672, 1, 'Wave-Song Girdle +7 SP'),
(8, 4, 29245, 10, 211, 11, 668, 1, 'Wave-Crest Striders +7 SP'),
(8, 4, 29259, 10, 1588, 11, 674, 1, 'Bracers of the Hunt +14 AP'),
(8, 4, 29261, 10, 1588, 11, 675, 1, 'Girdle of Ferocity +14 AP'),
(8, 4, 29262, 10, 1588, 11, 680, 1, 'Boots of the Endless Hunt +14 AP'),
(8, 4, 30534, 10, 1588, 11, 673, 1, 'Wyrmscale Greaves +14 AP'),
(8, 4, 30541, 10, 211, 11, 670, 1, 'Stormsong Kilt +7 SP'),
(8, 4, 32076, 10, 1588, 11, 679, 1, 'Handguards of the Steady +14 AP'),
(8, 4, 32077, 10, 211, 11, 667, 1, 'Wrath Infused Gauntlets +7 SP'),
(8, 4, 32078, 10, 211, 11, 669, 1, 'Pauldrons of Wild Magic +7 SP'),
-- Plate = 5
(8, 5, 29238, 10, 2730, 11, 667, 1, 'Lion\'s Heart Girdle +8 Dodge'),
(8, 5, 29239, 10, 2730, 11, 668, 1, 'Eaglecrest Warboots +8 Dodge'),
(8, 5, 29252, 10, 2730, 11, 671, 1, 'Bracers of Dignity +8 Dodge'),
(8, 5, 29253, 10, 2730, 11, 676, 1, 'Girdle of Valorous Deeds +8 Dodge'),
(8, 5, 29254, 10, 2730, 11, 678, 1, 'Boots of the Righteous Path +8 Dodge'),
(8, 5, 29463, 10, 2730, 11, 677, 1, 'Amber Bands of the Aggressor +8 Dodge'),
(8, 5, 30533, 10, 1588, 11, 679, 1, 'Vanquisher\'s Legplates +14 AP'),
(8, 5, 30536, 10, 1588, 11, 673, 1, 'Greaves of the Martyr +14 AP'),
(8, 5, 32072, 10, 2730, 11, 680, 1, 'Gauntlets of Dissension +8 Dodge'),
(8, 5, 32073, 10, 2730, 11, 674, 1, 'Spaulders of Dementia +8 Dodge'),
-- Weapons, Holdables, Shields = 6
(8, 6, 32082, 10, 2730, 11, 671, 1, 'The Fel Barrier +8 Dodge'),
(8, 6, 29362, 10, 2730, 11, 679, 1, 'The Sun Eater +8 Dodge'),
(8, 6, 29356, 10, 1588, 11, 676, 1, 'Quantum Blade +14 AP'),
(8, 6, 29355, 10, 211, 11, 674, 1, 'Terokk\'s Shadowstaff +7 SP'),
(8, 6, 29359, 10, 1588, 11, 680, 1, 'Feral Staff of Lashing +14 AP'),
(8, 6, 29348, 10, 1588, 11, 678, 1, 'The Bladefist +14 AP'),
(8, 6, 29346, 10, 1588, 11, 667, 1, 'Feltooth Eviscerator +14 AP'),
(8, 6, 29360, 10, 1588, 11, 681, 1, 'Vileblade of the Betrayer +14 AP'),
(8, 6, 29350, 10, 211, 11, 670, 1, 'The Black Stalk +7 SP'),
(8, 6, 29351, 10, 1588, 11, 677, 1, 'Wrathtide Longbow +14 AP'),
(8, 6, 29353, 10, 211, 11, 675, 1, 'Shockwave Truncheon +7 SP'),

-- TYPE_RAID_T4 = 9;
-- Back, Finger, Trinket, Neck = 1
(9, 1, 28797, 10, 211, 11, 692, 1, 'Brute Cloak of the Ogre-Magi +7 SP'),
(9, 1, 28830, 10, 1588, 11, 692, 1, 'Dragonspine Trophy +14 AP'),
(9, 1, 28823, 10, 211, 11, 692, 1, 'Eye of Gruul +7 SP'),
(9, 1, 28822, 10, 211, 11, 692, 1, 'Teeth of Gruul +7 SP'),
(9, 1, 28777, 10, 211, 11, 693, 1, 'Cloak of the Pit Stalker +7 SP'),
(9, 1, 28789, 10, 211, 11, 693, 1, 'Eye of Magtheridon +7 SP'),
-- Cloth = 2
(9, 2, 28799, 10, 211, 11, 692, 1, 'Belt of Divine Inspiration +7 SP'),
(9, 2, 28804, 10, 211, 11, 692, 1, 'Collar of Cho\'gall +7 SP'),
(9, 2, 28780, 10, 211, 11, 693, 1, 'Soul-Eater\'s Handwraps +7 SP'),
-- Leather = 3
(9, 3, 28796, 10, 1588, 11, 692, 1, 'Malefic Mask of the Shadows +14 AP'),
(9, 3, 28803, 10, 211, 11, 692, 1, 'Cowl of Nature\'s Breath +7 SP'),
(9, 3, 28828, 10, 1588, 11, 692, 1, 'Gronn-Stitched Girdle +14 AP'),
(9, 3, 28776, 10, 211, 11, 693, 1, 'Liar\'s Tongue Gloves +14 AP'),
-- Mail = 4
(9, 4, 28801, 10, 1588, 11, 692, 1, 'Maulgar\'s Warhelm +14 AP'),
(9, 4, 28827, 10, 1588, 11, 692, 1, 'Gauntlets of the Dragonslayer +14 AP'),
(9, 4, 28810, 10, 211, 11, 692, 1, 'Windshear Boots +7 SP'),
(9, 4, 28778, 10, 1588, 11, 693, 1, 'Terror Pit Girdle +14 AP'),
-- Plate = 5
(9, 5, 28795, 10, 1588, 11, 692, 1, 'Bladespire Warbands +14 AP'),
(9, 5, 28824, 10, 1588, 11, 692, 1, 'Gauntlets of Martial Perfection +14 AP'),
(9, 5, 28779, 10, 1588, 11, 693, 1, 'Girdle of the Endless Pit +14 AP'),
(9, 5, 28775, 10, 1588, 11, 693, 1, 'Thundering Greathelm +14 AP'),
-- Weapons, Holdables, Shields = 6
(9, 6, 28800, 10, 1588, 11, 692, 1, 'Hammer of the Naaru +14 AP'),
(9, 6, 28825, 10, 1588, 11, 692, 1, 'Aldori Legacy Defender +14 AP'),
(9, 6, 28794, 10, 1588, 11, 692, 1, 'Axe of the Gronn Lords +14 AP'),
(9, 6, 28802, 10, 211, 11, 692, 1, 'Bloodmaw Magus-Blade +7 SP'),
(9, 6, 28826, 10, 1588, 11, 692, 1, 'Shuriken of Negation +14 AP'),
(9, 6, 29458, 10, 211, 11, 693, 1, 'Aegis of the Vindicator +7 SP'),
(9, 6, 28782, 10, 211, 11, 693, 1, 'Crystalheart Pulse-Staff +7 SP'),
(9, 6, 28783, 10, 211, 11, 693, 1, 'Eredar Wand of Obliteration +7 SP'),
(9, 6, 28774, 10, 1588, 11, 693, 1, 'Glaive of the Pit +14 AP'),
(9, 6, 28781, 10, 211, 11, 693, 1, 'Karaborian Talisman +7 SP');
