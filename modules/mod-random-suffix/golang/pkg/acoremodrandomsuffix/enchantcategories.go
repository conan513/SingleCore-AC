// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

package acoremodrandomsuffix

//go:generate go run github.com/abice/go-enum --marshal --sql --names --noprefix --prefix EC
/*
ENUM(
MeleeStrDPS
MeleeStrTank
MeleeAgiDPS
MeleeAgiTank
RangedAgi
Caster
)
*/
type EnchantCategory int

// WeaponAttributes returns the attributes that each enchant category uses for weapons.
// This is merely an estimate of what would be optimal on typical sets
// of attributes based on common wow char builds
func (e EnchantCategory) WeaponAttributes() Attributes {
	switch e {
	case ECMeleeStrDPS:
		return Attributes{AStrength, AAttackPower, AHaste, AHit, ACrit, AExpertise}
	case ECMeleeStrTank:
		return Attributes{AStrength, AStamina, AHaste, AHit, AExpertise, ADefenseRating, ADodge, AParry}
	case ECMeleeAgiDPS:
		return Attributes{AAgility, AAttackPower, AHaste, AHit, ACrit, AExpertise}
	case ECMeleeAgiTank:
		return Attributes{AAgility, AStamina, AHaste, AHit, AExpertise, ADefenseRating, ADodge}
	case ECRangedAgi:
		return Attributes{AAgility, AAttackPower, AHaste, AHit, ACrit}
	case ECCaster:
		return Attributes{AIntellect, ASpirit, ASpellPower, AHaste, AHit, ACrit}
	default:
		log.Panic("Should not reach here, enchant category is invalid", "ec", e)
		return nil
	}
}

func (e EnchantCategory) Mask() uint {
	if _, ok := _EnchantCategoryMap[e]; !ok {
		log.Panic("get mask failed, attribute is not valid", "e", e.String())
	}
	return 1 << e
}

type EnchantCategories []EnchantCategory

func (ee EnchantCategories) Mask() uint { return GetMask(ee) }
