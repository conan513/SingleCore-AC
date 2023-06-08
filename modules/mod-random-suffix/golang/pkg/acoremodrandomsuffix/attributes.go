// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

package acoremodrandomsuffix

import (
	_ "github.com/abice/go-enum/generator"
	"gonum.org/v1/gonum/stat/combin"
)

//go:generate go run github.com/abice/go-enum --marshal --sql --names --noprefix --prefix A
/*
ENUM(
Strength
Agility
Intellect
Spirit
Stamina
AttackPower
SpellPower
Haste
Hit
Crit
Expertise
DefenseRating
Dodge
Parry
)
*/
type Attribute uint

func (a Attribute) Mask() uint {
	if _, ok := _AttributeMap[a]; !ok {
		log.Panic("get mask failed, attribute is not valid", "a", a.String())
	}
	return 1 << a
}

var attributeToEnchantID map[Attribute]int32 = map[Attribute]int32{
	AStrength:      2805,
	AAgility:       2802,
	AIntellect:     2804,
	ASpirit:        2806,
	AStamina:       2803,
	AAttackPower:   2825,
	ASpellPower:    2824,
	AHaste:         3726,
	AHit:           3727,
	ACrit:          2822,
	AExpertise:     9991,
	ADefenseRating: 2813,
	ADodge:         2815,
	AParry:         9992,
}

func (a Attribute) EnchantID() int32 {
	enchID, ok := attributeToEnchantID[a]
	if !ok {
		log.Panic("Attribute is invalid", "a", a.String())
	}
	return enchID
}

type Attributes []Attribute

func (aa Attributes) EnchantIDs() []int32 {
	ee := make([]int32, 0, len(aa))
	for _, a := range aa {
		ee = append(ee, a.EnchantID())
	}
	return ee
}

func (aa Attributes) Mask() uint { return GetMask(aa) }

func (aa Attributes) IsValid() bool {
	m := aa.Mask()
	isAgi := AAgility.Mask()&m > 0
	isStr := AStrength.Mask()&m > 0
	isSpirit := ASpirit.Mask()&m > 0
	isInt := AIntellect.Mask()&m > 0
	isExpertise := AExpertise.Mask()&m > 0
	isSP := ASpellPower.Mask()&m > 0
	isAP := AAttackPower.Mask()&m > 0
	isParry := AParry.Mask()&m > 0
	isDefense := ADefenseRating.Mask()&m > 0
	isDodge := ADodge.Mask()&m > 0

	isDefensive := isDefense || isParry || isDodge

	switch {
	case isAgi && isStr && isInt:
		// make the combi for agi/str/int impossible
		return false
	case (isAgi || isStr) && isSpirit:
		// Make agi and str gear dont have spirit
		return false
	case isAgi && isParry:
		// make agi not be able to roll for parry
		return false
	case ((isSpirit || isInt || isSP) && !isAgi && !isStr) && (isAP || isDefensive || isExpertise):
		// Make spirit && int gear have no melee stuff
		return false
	case isAgi && !isStr && (isInt || isSP) && isDefensive:
		// Make hybrid agi/int/SP gear ! have any defensive stuff
		return false
	case isAgi && !isStr && isDefensive && isExpertise:
		// Agi gear with defensive status shouldnt roll expertise
		return false
	}
	return true
}

func generateAttributeCombinations(n int, attrsToChoose Attributes) []Attributes {
	// The integer slices returned from Combinations can be used to index
	// into a data structure.
	var attrss []Attributes
	cs := combin.Combinations(len(attrsToChoose), n)
	for _, c := range cs {
		attrs := make(Attributes, 0, n)
		for _, attrIdx := range c {
			attrs = append(attrs, attrsToChoose[attrIdx])
		}
		attrss = append(attrss, attrs)
	}
	return attrss
}
