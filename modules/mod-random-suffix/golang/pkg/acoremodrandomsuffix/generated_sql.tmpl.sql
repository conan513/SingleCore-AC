-- Delete custom item enchant IDs first
DELETE FROM spellitemenchantment_dbc WHERE ID IN ({{range $i, $e := .CustomSpellItemEnchantmentEntries -}}{{$e.ID}}{{if not (last $i $.CustomSpellItemEnchantmentEntries)}},{{end}}{{end }});
INSERT INTO spellitemenchantment_dbc (ID,Effect_1,EffectArg_1,Name_Lang_enUS,Name_Lang_Mask)
VALUES
{{range $i, $e := .CustomSpellItemEnchantmentEntries -}}
({{$e.ID}},{{$e.Effect_1}},{{$e.EffectArg_1}},'{{$e.Name_Lang_enUS}}',{{$e.Name_Lang_Mask}}){{if last $i $.CustomSpellItemEnchantmentEntries}};{{else}},{{end}}
{{end }}

-- Delete the custom random suffixes first
DELETE FROM itemrandomsuffix_dbc where ID >= {{.CustomItemRandomSuffixStartID}} AND ID <= {{.CustomItemRandomSuffixEndID}};
INSERT INTO itemrandomsuffix_dbc (ID,Name_Lang_enUS,Name_Lang_Mask,InternalName,Enchantment_1,Enchantment_2,Enchantment_3,Enchantment_4,Enchantment_5,AllocationPct_1,AllocationPct_2,AllocationPct_3,AllocationPct_4,AllocationPct_5)
VALUES
{{range $i, $e := .CustomItemRandomSuffixEntries -}}
({{$e.ID}},'{{$e.Name_Lang_enUS}}',{{$e.Name_Lang_Mask}},'{{$e.InternalName}}',{{$e.Enchantment_1}},{{$e.Enchantment_2}},{{$e.Enchantment_3}},{{$e.Enchantment_4}},{{$e.Enchantment_5}},{{$e.AllocationPct_1}},{{$e.AllocationPct_2}},{{$e.AllocationPct_3}},{{$e.AllocationPct_4}},{{$e.AllocationPct_5}}){{if last $i $.CustomItemRandomSuffixEntries}};{{else}},{{end}}
{{end }}

-- Add in new enchants
DROP TABLE IF EXISTS `item_enchantment_random_suffixes`;
CREATE TABLE `item_enchantment_random_suffixes` (
  `SuffixID` int NOT NULL DEFAULT '0',
  `MinLevel` int DEFAULT '0',
  `MaxLevel` int DEFAULT '0',
  `AttributeMask` int unsigned DEFAULT '0',
  `ItemClass` int unsigned DEFAULT '0',
  `ItemSubClassMask` int unsigned DEFAULT '0',
  `EnchantQuality` int unsigned DEFAULT '0',
  `EnchantCategoryMask` int unsigned DEFAULT '0',
  PRIMARY KEY (`SuffixID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
INSERT INTO item_enchantment_random_suffixes (SuffixID,MinLevel,MaxLevel,AttributeMask,ItemClass,ItemSubClassMask,EnchantQuality,EnchantCategoryMask)
VALUES
{{range $i, $e := .ItemEnchantmentRandomSuffixes -}}
({{$e.ID}},{{$e.MinLevel}},{{$e.MaxLevel}},{{$e.AttrMask}},{{$e.ItemClass}},{{$e.ItemSubclassMask}},{{$e.EnchantQuality}},{{$e.EnchCatMask}}){{if last $i $.CustomItemRandomSuffixEntries}};{{else}},{{end}}
{{end }}
