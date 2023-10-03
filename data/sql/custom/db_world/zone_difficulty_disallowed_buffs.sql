DROP TABLE IF EXISTS `zone_difficulty_disallowed_buffs`;
CREATE TABLE `zone_difficulty_disallowed_buffs` (
    `MapID` INT NOT NULL DEFAULT 0,
    `DisallowedBuffs` TEXT,
    `Enabled` TINYINT DEFAULT 1,
	`Comment` TEXT,
    PRIMARY KEY (`MapID`)
);

DELETE FROM `zone_difficulty_disallowed_buffs`;
INSERT INTO `zone_difficulty_disallowed_buffs` (`MapID`, `DisallowedBuffs`, `Enabled`, `Comment`) VALUES
# BGs
(30, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Alterac Valley: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(489, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Warsong Gulch: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(529, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Arathi Basin: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(556, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Eye of the Storm: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(559, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Ring of Trials: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(562, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Blade\'s Edge Arena: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(572, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Ruins of Lordaeron: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(617, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Dalaran Arena: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(618, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Ring of Valor: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
# Raids
# (229, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Blackrock Spire: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(409, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Molten Core: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(249, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Onyxia\'s Lair: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(469, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Blackwing Lair: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(509, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Ruins of Ahn\'Qiraj: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(309, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Zul Gurub: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder'),
(531, '15366 16609 22888 24425 22817 22818 22820 15123', 1, 'Forbid in Temple of Ahn\'Qiraj: Songflower Serenade, Warchiefs Blessing, Rallying Cry of the Dragonslayer, Spirit of Zandalar, Fengus\' Ferocity\', Mol\'dar\'s Moxie, Slip\'kik\'s Savvy, Resist Fire from Scarshield Spellbinder');
