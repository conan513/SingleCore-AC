DROP TABLE IF EXISTS `account_bound_factionchange_spells`;
CREATE TABLE `account_bound_factionchange_spells` (
	`alliance_id` INT(10) UNSIGNED NOT NULL,
	`alliance_comment` TEXT NOT NULL COLLATE 'utf8mb4_general_ci',
	`horde_id` INT(10) UNSIGNED NOT NULL,
	`horde_comment` TEXT NOT NULL COLLATE 'utf8mb4_general_ci',
	PRIMARY KEY (`alliance_id`, `horde_id`) USING BTREE
)
COLLATE='utf8mb4_general_ci'
ENGINE=MyISAM
;

-- Companions
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (62609, 'Argent Squire', 62746, 'Argent Gruntling');

-- Mounts
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (458, 'Brown Horse', 6654, 'Brown Wolf');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (470, 'Black Stallion', 64658, 'Black Wolf');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (472, 'Pinto', 580, 'Timber Wolf');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (6648, 'Chestnut Mare', 6653, 'Dire Wolf');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (6777, 'Gray Ram', 8395, 'Emerald Raptor');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (6898, 'White Ram', 10796, 'Turquoise Raptor');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (6899, 'Brown Ram', 10799, 'Violet Raptor');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (8394, 'Striped Frostsaber', 64977, 'Black Skeletal Horse');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (10789, 'Spotted Frostsaber', 17464, 'Brown Skeletal Horse');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (10793, 'Striped Nightsaber', 17463, 'Blue Skeletal Horse');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (10873, 'Red Mechanostrider', 64657, 'White Kodo');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (10969, 'Blue Mechanostrider', 35020, 'Blue Hawkstrider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (15779, 'White Mechanostrider Mod B', 18992, 'Teal Kodo');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (16082, 'Palomino', 16080, 'Red Wolf');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (16083, 'White Stallion', 16081, 'Winter Wolf');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (17229, 'Winterspring Frostsaber', 64659, 'Venomhide Ravasaur');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (17453, 'Green Mechanostrider', 18989, 'Gray Kodo');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (17459, 'Icy Blue Mechanostrider Mod A', 18991, 'Green Kodo');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (17460, 'Frost Ram', 17450, 'Ivory Raptor');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (17461, 'Black Ram', 16084, 'Mottled Red Raptor');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (22717, 'Black War Steed', 22724, 'Black War Wolf');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (22719, 'Black Battlestrider', 22718, 'Black War Kodo');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (22720, 'Black War Ram', 22721, 'Black War Raptor');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (22723, 'Black War Tiger', 22722, 'Red Skeletal Warhorse');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (23219, 'Swift Mistsaber', 23246, 'Purple Skeletal Warhorse');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (23221, 'Swift Frostsaber', 66846, 'Ochre Skeletal Warhorse');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (23222, 'Swift Yellow Mechanostrider', 23247, 'Great White Kodo');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (23223, 'Swift White Mechanostrider', 23248, 'Great Gray Kodo');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (23225, 'Swift Green Mechanostrider', 23249, 'Great Brown Kodo');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (23227, 'Swift Palomino', 23251, 'Swift Timber Wolf');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (23228, 'Swift White Steed', 23252, 'Swift Gray Wolf');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (23229, 'Swift Brown Steed', 23250, 'Swift Brown Wolf');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (23238, 'Swift Brown Ram', 23243, 'Swift Orange Raptor');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (23239, 'Swift Gray Ram', 23241, 'Swift Blue Raptor');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (23240, 'Swift White Ram', 23242, 'Swift Olive Raptor');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (23338, 'Swift Stormsaber', 17465, 'Green Skeletal Warhorse');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (23510, 'Stormpike Battle Charger', 23509, 'Frostwolf Howler');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (32235, 'Golden Gryphon', 32245, 'Green Wind Rider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (32239, 'Ebon Gryphon', 32243, 'Tawny Wind Rider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (32240, 'Snowy Gryphon', 32244, 'Blue Wind Rider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (32242, 'Swift Blue Gryphon', 32296, 'Swift Yellow Wind Rider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (32289, 'Swift Red Gryphon', 32246, 'Swift Red Wind Rider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (32290, 'Swift Green Gryphon', 32295, 'Swift Green Wind Rider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (32292, 'Swift Purple Gryphon', 32297, 'Swift Purple Wind Rider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (34406, 'Brown Elekk', 35022, 'Black Hawkstrider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (35710, 'Gray Elekk', 34795, 'Red Hawkstrider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (35711, 'Purple Elekk', 35018, 'Purple Hawkstrider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (35712, 'Great Green Elekk', 35027, 'Swift Purple Hawkstrider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (35713, 'Great Blue Elekk', 35025, 'Swift Green Hawkstrider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (35714, 'Great Purple Elekk', 33660, 'Swift Pink Hawkstrider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (48027, 'Black War Elekk', 35028, 'Swift Warstrider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (59785, 'Black War Mammoth', 59788, 'Black War Mammoth');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (59791, 'Wooly Mammoth', 59793, 'Wooly Mammoth');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (59799, 'Ice Mammoth', 59797, 'Ice Mammoth');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (60114, 'Armored Brown Bear', 60116, 'Armored Brown Bear');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (60118, 'Black War Bear', 60119, 'Black War Bear');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (60424, 'Mekgineer\'s Chopper', 55531, 'Mechano-hog');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (61229, 'Armored Snowy Gryphon', 61230, 'Armored Blue Wind Rider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (61425, 'Traveler\'s Tundra Mammoth', 61447, 'Traveler\'s Tundra Mammoth');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (61470, 'Grand Ice Mammoth', 61469, 'Grand Ice Mammoth');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (61996, 'Blue Dragonhawk', 61997, 'Red Dragonhawk');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (63232, 'Stormwind Steed', 63640, 'Orgrimmar Wolf');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (63636, 'Ironforge Ram', 63635, 'Darkspear Raptor');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (63637, 'Darnassian Nightsaber', 63643, 'Forsaken Warhorse');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (63638, 'Gnomeregan Mechanostrider', 63641, 'Thunder Bluff Kodo');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (63639, 'Exodar Elekk', 63642, 'Silvermoon Hawkstrider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (65637, 'Great Red Elekk', 65639, 'Swift Red Hawkstrider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (65638, 'Swift Moonsaber', 65645, 'White Skeletal Warhorse');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (65640, 'Swift Gray Steed', 65646, 'Swift Burgundy Wolf');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (65642, 'Turbostrider', 65641, 'Great Golden Kodo');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (65643, 'Swift Violet Ram', 65644, 'Swift Purple Raptor');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (66087, 'Silver Covenant Hippogryph', 66088, 'Sunreaver Dragonhawk');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (66090, 'Quel\'dorei Steed', 66091, 'Sunreaver Hawkstrider');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (66847, 'Striped Dawnsaber', 17462, 'Red Skeletal Horse');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (68057, 'Swift Alliance Steed', 68056, 'Swift Horde Wolf');
INSERT INTO `account_bound_factionchange_spells` (`alliance_id`, `alliance_comment`, `horde_id`, `horde_comment`) VALUES (68187, 'Crusader\'s White Warhorse', 68188, 'Crusader\'s Black Warhorse');
