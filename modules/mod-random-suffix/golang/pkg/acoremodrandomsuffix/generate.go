// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

package acoremodrandomsuffix

import (
	"fmt"
	"math"
	"sort"
	"strconv"

	"github.com/lohvht/mod-random-suffix/golang/pkg/dbc"
	"github.com/pkg/errors"
)

const maxEnchantsForSuffix = 5

var (
	ErrMaxEnchantIDs                     = fmt.Errorf("number of enchants passed into append item random suffix exceeds the maximum of %d", maxEnchantsForSuffix)
	ErrEnchantsAndAllocationPctNotMatch  = errors.New("The number of enchant IDs and allocation pcts passed in are not equal")
	ErrExceededItemRandomSuffixMaxAmount = fmt.Errorf("Exceeded maximum IDs, the highest ID number should be %d", math.MaxInt16)
)

type customRandomSuffixEntry struct {
	ID               int32
	Name             string
	EnchantIDs       []int32
	AllocationPcts   []int32
	AttrMask         uint
	ItemSubclassMask uint
	EnchCatMask      uint
	EnchantQuality   int
	MinLevel         int
	MaxLevel         int
	ItemClass        int
}

func (e customRandomSuffixEntry) displayName() string {
	return fmt.Sprintf("[%s]", e.Name)
}

func (e customRandomSuffixEntry) internalName() string {
	return fmt.Sprint("CUSTOM ", e.Name)
}

func (e customRandomSuffixEntry) check() error {
	if len(e.EnchantIDs) != len(e.AllocationPcts) {
		return errors.Wrapf(ErrEnchantsAndAllocationPctNotMatch, "enchIDs: %v, allocPcts: %v", e.EnchantIDs, e.AllocationPcts)
	}
	if len(e.EnchantIDs) > maxEnchantsForSuffix {
		return errors.Wrapf(ErrMaxEnchantIDs, "enchIDs: %v, allocPcts: %v", e.EnchantIDs, e.AllocationPcts)
	}
	if len(e.AllocationPcts) > maxEnchantsForSuffix {
		return errors.Wrapf(ErrMaxEnchantIDs, "enchIDs: %v, allocPcts: %v", e.EnchantIDs, e.AllocationPcts)
	}
	return nil
}

func (e customRandomSuffixEntry) nameMask() int64 {
	return 16712190
}

func (e customRandomSuffixEntry) toDBCCSVEntry(schema dbc.DBCSchema) ([]string, error) {
	err := e.check()
	if err != nil {
		return nil, err
	}
	// Form the item random suffix here
	row := make([]string, len(schema.Fields))
	// ID
	row[0] = strconv.FormatInt(int64(e.ID), 10)
	// Name
	row[1] = e.displayName()
	// NameMask
	row[17] = strconv.FormatInt(e.nameMask(), 10)
	// InternalName
	row[18] = e.internalName()
	newEnchantIDs := make([]int32, maxEnchantsForSuffix)
	newAllocationPcts := make([]int32, maxEnchantsForSuffix)
	copy(newEnchantIDs, e.EnchantIDs)
	copy(newAllocationPcts, e.AllocationPcts)
	// EnchantIDs
	for i, enchID := range newEnchantIDs {
		row[i+19] = strconv.FormatInt(int64(enchID), 10)
	}
	for i, allocPct := range newAllocationPcts {
		row[i+24] = strconv.FormatInt(int64(allocPct), 10)
	}
	return row, nil
}

func (e customRandomSuffixEntry) toTmplSQLEntry() tmplItemRandomSuffixEntry {
	enchIDs := make([]int32, 5)
	allocPcts := make([]int32, 5)
	for i, enchID := range e.EnchantIDs {
		enchIDs[i] = enchID
	}
	for i, allocPct := range e.AllocationPcts {
		allocPcts[i] = allocPct
	}
	return tmplItemRandomSuffixEntry{
		ID:              e.ID,
		Name_Lang_enUS:  e.displayName(),
		Name_Lang_Mask:  e.nameMask(),
		InternalName:    e.internalName(),
		Enchantment_1:   enchIDs[0],
		Enchantment_2:   enchIDs[1],
		Enchantment_3:   enchIDs[2],
		Enchantment_4:   enchIDs[3],
		Enchantment_5:   enchIDs[4],
		AllocationPct_1: allocPcts[0],
		AllocationPct_2: allocPcts[1],
		AllocationPct_3: allocPcts[2],
		AllocationPct_4: allocPcts[3],
		AllocationPct_5: allocPcts[4],
	}
}

func generateItemRandomSuffixCSV(schema dbc.DBCSchema, es []customRandomSuffixEntry) ([][]string, error) {
	var irsDBCCSVHeader []string
	for fi := range schema.Fields {
		irsDBCCSVHeader = append(irsDBCCSVHeader, schema.FieldName(fi))
	}
	rows := [][]string{irsDBCCSVHeader}
	for i, e := range es {
		row, err := e.toDBCCSVEntry(schema)
		if err != nil {
			return nil, errors.Wrapf(err, "DBC entry for item random suffix cannot be converted to entry: i=%d", i)
		}
		rows = append(rows, row)
	}
	return rows, nil
}

var customSpellItemEnchantDBCCSVEntries = [][]string{
	{"ID", "Charges", "Effect_1", "Effect_2", "Effect_3", "EffectPointsMin_1", "EffectPointsMin_2", "EffectPointsMin_3", "EffectPointsMax_1", "EffectPointsMax_2", "EffectPointsMax_3", "EffectArg_1", "EffectArg_2", "EffectArg_3", "Name_Lang_enUS", "Name_Lang_enGB", "Name_Lang_koKR", "Name_Lang_frFR", "Name_Lang_deDE", "Name_Lang_enCN", "Name_Lang_zhCN", "Name_Lang_enTW", "Name_Lang_zhTW", "Name_Lang_esES", "Name_Lang_esMX", "Name_Lang_ruRU", "Name_Lang_ptPT", "Name_Lang_ptBR", "Name_Lang_itIT", "Name_Lang_Unk", "Name_Lang_Mask", "ItemVisual", "Flags", "Src_ItemID", "Condition_Id", "RequiredSkillID", "RequiredSkillRank", "MinLevel"},
	{strconv.FormatInt(int64(AExpertise.EnchantID()), 10), "0", "5", "0", "0", "0", "0", "0", "0", "0", "0", "37", "0", "0", "+$i Expertise Rating", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "16712190", "0", "0", "0", "0", "0", "0", "0"},
	{strconv.FormatInt(int64(AParry.EnchantID()), 10), "0", "5", "0", "0", "0", "0", "0", "0", "0", "0", "14", "0", "0", "+$i Parry Rating", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "16712190", "0", "0", "0", "0", "0", "0", "0"},
}

var latinNumerals = []string{"I", "II", "III", "IV", "V"}

// Generate generates custom suffixes and other DBC changes needed for this random suffix mod.
// It appends to the itemRandomSuffix and spellItemEnchant DBC passed into this function
func Generate(p *ProcessedConfig) error {
	err := p.SrcSpellItemEnchantDBC.AppendFromCSVData(customSpellItemEnchantDBCCSVEntries)
	if err != nil {
		return errors.Wrap(err, "unable to append spell item enchant DBC with custom expertise and parry entries")
	}
	var irsDBCCSVHeader []string
	for fi := range p.SrcItemSuffixDBC.Schema.Fields {
		irsDBCCSVHeader = append(irsDBCCSVHeader, p.SrcItemSuffixDBC.Schema.FieldName(fi))
	}
	var suffEntries []customRandomSuffixEntry
	irsDBCID := p.ItemRandomSuffixDBCCustomStartID
	seenNames := make(map[string]struct{})
	for i := 0; i < p.NumberOfAttributes; i++ {
		combis := generateAttributeCombinations(i+1, allAttributes)
		for _, combi := range combis {
			if !combi.IsValid() {
				continue
			}
			eIDs := combi.EnchantIDs()
			attrMask := combi.Mask()
			name := p.SuffixMaskToNames()[attrMask]
			seenNames[name] = struct{}{}
			for k, maxAllocPoint := range p.StatPointAllocTiers {
				allocPcts := make([]int32, len(eIDs))
				for i := range allocPcts {
					allocPcts[i] = int32(maxAllocPoint) / int32(len(eIDs))
				}
				suffEntries = append(suffEntries, customRandomSuffixEntry{
					ID:             irsDBCID,
					Name:           fmt.Sprintf("%s %s", name, latinNumerals[k]),
					EnchantIDs:     eIDs,
					AllocationPcts: allocPcts,
					AttrMask:       attrMask,
					EnchantQuality: k,
				})
				irsDBCID++
			}
		}
	}
	for n := range p.Suffixes {
		if _, ok := seenNames[n]; !ok {
			log.Warn("This name hasnt been seen", "suffix_name", n)
		}
	}

	weaponEnchantIDs := make([]int32, 0, len(p.WeaponSuffixes))
	for enchID := range p.WeaponSuffixes {
		weaponEnchantIDs = append(weaponEnchantIDs, enchID)
	}
	sort.Slice(weaponEnchantIDs, func(i, j int) bool { return weaponEnchantIDs[i] < weaponEnchantIDs[j] })
	for _, enchID := range weaponEnchantIDs {
		ws, ok := p.WeaponSuffixes[enchID]
		if !ok {
			log.Panic("Weapon suffix iteration, enchant ID not found even if weaponSuffixes should have this ID", "enchID", enchID)
		}
		// Base
		enchIDs := []int32{enchID}
		allocPcts := []int32{1}
		suffEntries = append(suffEntries, customRandomSuffixEntry{
			ID:               irsDBCID,
			Name:             ws.Name,
			EnchantIDs:       enchIDs,
			AllocationPcts:   allocPcts,
			AttrMask:         0, // No additional attributes
			ItemSubclassMask: ws.ItemSubClasses.Mask(),
			EnchCatMask:      ws.EnchantCategories.Mask(),
			EnchantQuality:   0, // lowest enchant quality
			MinLevel:         ws.MinLevel,
			MaxLevel:         ws.MaxLevel,
			ItemClass:        ws.ItemClass,
		})
		irsDBCID++
		seenWeaponAttrMaskToSuffEntriesIdx := make(map[uint][]int)
		for _, ec := range ws.EnchantCategories {
			for _, combi := range generateAttributeCombinations(p.NumberOfAttributesWeapons, ec.WeaponAttributes()) {
				if !combi.IsValid() {
					continue
				}
				attrMask := combi.Mask()
				// This will still make a lot of duplicates but better to have duplicates so that our queries will be easier
				if suffEntriesIdxs, ok := seenWeaponAttrMaskToSuffEntriesIdx[attrMask]; ok {
					for _, idx := range suffEntriesIdxs {
						suffEntries[idx].EnchCatMask |= ec.Mask()
					}
					continue
				}
				combiEnchIDs := combi.EnchantIDs()
				for k, maxAllocPoint := range p.StatPointAllocTiers {
					if k == len(p.StatPointAllocTiers)-1 {
						// Dont include the last allocation thresholds for weapons.
						continue
					}
					weaponMaxAllocPct := int(math.Ceil(p.WeaponStatPenalty * float64(maxAllocPoint)))
					combiAllocPcts := make([]int32, len(combiEnchIDs))
					for i := range combiAllocPcts {
						combiAllocPcts[i] = int32(weaponMaxAllocPct) / int32(len(combiEnchIDs))
					}
					suffEntries = append(suffEntries, customRandomSuffixEntry{
						ID:               irsDBCID,
						Name:             fmt.Sprintf("%s - %s %s", ws.Name, p.SuffixMaskToNames()[attrMask], latinNumerals[k]),
						EnchantIDs:       append(enchIDs, combiEnchIDs...),
						AllocationPcts:   append(allocPcts, combiAllocPcts...),
						AttrMask:         attrMask,
						EnchantQuality:   k,
						EnchCatMask:      ec.Mask(),
						ItemSubclassMask: ws.ItemSubClasses.Mask(),
						MinLevel:         ws.MinLevel,
						MaxLevel:         ws.MaxLevel,
						ItemClass:        ws.ItemClass,
					})
					seenWeaponAttrMaskToSuffEntriesIdx[attrMask] = append(seenWeaponAttrMaskToSuffEntriesIdx[attrMask], len(suffEntries)-1)
					irsDBCID++
				}
			}
		}
	}

	itemRandomSuffixRows, err := generateItemRandomSuffixCSV(p.SrcItemSuffixDBC.Schema, suffEntries)
	if err != nil {
		return errors.Wrap(err, "Error generate item random suffix DBC CSV entries")
	}
	err = p.SrcItemSuffixDBC.AppendFromCSVData(itemRandomSuffixRows)
	if err != nil {
		return errors.Wrap(err, "Error appending to item random suffix DBC")
	}
	if irsDBCID > math.MaxInt16 {
		return errors.Wrapf(ErrExceededItemRandomSuffixMaxAmount, "last ID was: %d", irsDBCID)
	}

	var tmplSpellItemEnchEntries []tmplSpellItemEnchantmentEntry
	for _, e := range customSpellItemEnchantDBCCSVEntries[1:] {
		tmplSpellItemEnchEntries = append(tmplSpellItemEnchEntries, tmplSpellItemEnchantmentEntry{
			ID: e[0], Effect_1: e[2], EffectArg_1: e[11], Name_Lang_enUS: e[14], Name_Lang_Mask: e[30],
		})
	}
	var tmplRanSuffEntries []tmplItemRandomSuffixEntry
	for _, e := range suffEntries {
		tmplRanSuffEntries = append(tmplRanSuffEntries, e.toTmplSQLEntry())
	}
	sqlData := tmplData{
		CustomItemRandomSuffixStartID:     p.ItemRandomSuffixDBCCustomStartID,
		CustomItemRandomSuffixEndID:       irsDBCID,
		CustomItemRandomSuffixEntries:     tmplRanSuffEntries,
		CustomSpellItemEnchantmentEntries: tmplSpellItemEnchEntries,
		ItemEnchantmentRandomSuffixes:     suffEntries,
	}
	err = generatedSQLTemplate.Execute(p.DstGeneratedWorldSQL, &sqlData)
	if err != nil {
		return err
	}
	err = p.SrcItemSuffixDBC.Export(p.DstExportedGeneratedItemSuffixDBC)
	if err != nil {
		return err
	}
	err = p.SrcSpellItemEnchantDBC.Export(p.DstExportedGeneratedSpellItemEnchantDBC)
	if err != nil {
		return err
	}
	return nil
}
