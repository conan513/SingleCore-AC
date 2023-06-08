// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

package acoremodrandomsuffix

import (
	"io"

	"github.com/lohvht/mod-random-suffix/golang/pkg/dbc"
	"github.com/pkg/errors"
	"gopkg.in/yaml.v3"
)

var (
	ErrConfigHasDuplicateAttributeMix = errors.New("Entry of the same mask already exists, check the suffixes again")
)

type WeaponSuffix struct {
	Name              string            `yaml:"name"`
	MinLevel          int               `yaml:"minlvl"`
	MaxLevel          int               `yaml:"maxlvl"`
	ItemClass         int               `yaml:"itemclass"`
	ItemSubClasses    ItemSubclasses    `yaml:"itemsubclasses"`
	EnchantCategories EnchantCategories `yaml:"enchantcats"`
}

// Config contains the configuration of suffix generation taken from yaml.
type Config struct {
	// Source
	SrcItemSuffixDBC          string `yaml:"src-item-suffix-dbc"`
	SrcItemSuffixSchema       string `yaml:"src-item-suffix-schema"`
	SrcSpellItemEnchantDBC    string `yaml:"src-spell-item-enchant-dbc"`
	SrcSpellItemEnchantSchema string `yaml:"src-spell-item-enchant-schema"`
	// Destination
	DstGeneratedWorldSQL            string `yaml:"dst-generated-world-sql"`
	DstGeneratedItemSuffixDBC       string `yaml:"dst-generated-item-suffix-dbc"`
	DstGeneratedSpellItemEnchantDBC string `yaml:"dst-generated-spell-item-enchant-dbc"`
	// Generate configuration
	ItemRandomSuffixDBCCustomStartID int32                  `yaml:"item-random-suffix-dbc-custom-start-id"`
	NumberOfAttributes               int                    `yaml:"number-of-attributes"`
	NumberOfAttributesWeapons        int                    `yaml:"number-of-attributes-weapons"`
	StatPointAllocTiers              []int                  `yaml:"stat-point-alloc-tiers"`
	WeaponStatPenalty                float64                `yaml:"weapon-stat-penalty"`
	Suffixes                         map[string]Attributes  `yaml:"suffixes"`
	WeaponSuffixes                   map[int32]WeaponSuffix `yaml:"weaponsuffixes"`
}

// ProcessFromReader processes the suffix generation config from a reader and spits out a processed config
func (c *Config) ProcessFromReader(cfgfile io.Reader) (*ProcessedConfig, error) {
	ymlCfgFile := yaml.NewDecoder(cfgfile)
	err := ymlCfgFile.Decode(c)
	if err != nil {
		return nil, errors.Wrap(err, "unable to marshal config")
	}
	irsDBC, err := dbc.NewDBCFromFile(c.SrcItemSuffixDBC, c.SrcItemSuffixSchema)
	if err != nil {
		return nil, errors.Wrapf(err, "Error open source ItemSuffix DBC - dbcpath '%s', schemapath '%s'", c.SrcItemSuffixDBC, c.SrcItemSuffixSchema)
	}
	sieDBC, err := dbc.NewDBCFromFile(c.SrcSpellItemEnchantDBC, c.SrcSpellItemEnchantSchema)
	if err != nil {
		return nil, errors.Wrapf(err, "Error open source SpellItemEnchantment DBC - dbcpath '%s', schemapath '%s'", c.SrcSpellItemEnchantDBC, c.SrcSpellItemEnchantSchema)
	}
	// Destination DBCs
	dstWorldSQL, err := mkDirAndOpenFile(c.DstGeneratedWorldSQL)
	if err != nil {
		return nil, errors.Wrapf(err, "error open destination world SQL, path - '%s'", c.DstGeneratedWorldSQL)
	}
	dstIrsDBC, err := mkDirAndOpenFile(c.DstGeneratedItemSuffixDBC)
	if err != nil {
		return nil, errors.Wrapf(err, "error open destination ItemSuffix DBC, path - '%s'", c.DstGeneratedItemSuffixDBC)
	}
	dstSieDBC, err := mkDirAndOpenFile(c.DstGeneratedSpellItemEnchantDBC)
	if err != nil {
		return nil, errors.Wrapf(err, "error open destination SpellItemEnchantment DBC, path - '%s'", c.DstGeneratedSpellItemEnchantDBC)
	}

	p := &ProcessedConfig{
		SrcItemSuffixDBC:                        &irsDBC,
		SrcSpellItemEnchantDBC:                  &sieDBC,
		DstGeneratedWorldSQL:                    dstWorldSQL,
		DstExportedGeneratedItemSuffixDBC:       dstIrsDBC,
		DstExportedGeneratedSpellItemEnchantDBC: dstSieDBC,
		ItemRandomSuffixDBCCustomStartID:        c.ItemRandomSuffixDBCCustomStartID,
		NumberOfAttributes:                      c.NumberOfAttributes,
		NumberOfAttributesWeapons:               c.NumberOfAttributesWeapons,
		StatPointAllocTiers:                     c.StatPointAllocTiers,
		WeaponStatPenalty:                       c.WeaponStatPenalty,
		Suffixes:                                c.Suffixes,
		WeaponSuffixes:                          c.WeaponSuffixes,
	}
	// set this once, to make sure that its valid
	p.SuffixMaskToNames()
	return p, nil
}

type ProcessedConfig struct {
	// Source
	SrcItemSuffixDBC       *dbc.DBC
	SrcSpellItemEnchantDBC *dbc.DBC
	// Destination
	DstGeneratedWorldSQL                    io.Writer
	DstExportedGeneratedItemSuffixDBC       io.WriteSeeker
	DstExportedGeneratedSpellItemEnchantDBC io.WriteSeeker
	// Generate configuration
	ItemRandomSuffixDBCCustomStartID int32
	NumberOfAttributes               int
	NumberOfAttributesWeapons        int
	StatPointAllocTiers              []int
	WeaponStatPenalty                float64
	Suffixes                         map[string]Attributes
	WeaponSuffixes                   map[int32]WeaponSuffix

	// generated from Suffixes to check duplicates
	suffixMaskToNames map[uint]string `yaml:"-"`
}

func (p *ProcessedConfig) SuffixMaskToNames() map[uint]string {
	if p.suffixMaskToNames != nil {
		return p.suffixMaskToNames
	}
	suffixMaskToNames := make(map[uint]string)
	for name, attrs := range p.Suffixes {
		m := attrs.Mask()
		if existingName, ok := suffixMaskToNames[m]; ok {
			log.Panic(ErrConfigHasDuplicateAttributeMix.Error(), "entry_name", name, "existing_name", existingName)
		}
		suffixMaskToNames[m] = name
	}
	p.suffixMaskToNames = suffixMaskToNames
	return p.suffixMaskToNames
}
