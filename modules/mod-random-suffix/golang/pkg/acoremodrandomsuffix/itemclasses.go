// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

package acoremodrandomsuffix

import (
	_ "github.com/abice/go-enum/generator"
)

//go:generate go run github.com/abice/go-enum --marshal --sql --names --noprefix --prefix IWSCT
/*
ENUM(
Axe
Axe2
Bow
Gun
Mace
Mace2
Polearm
Sword
Sword2
Obsolete
Staff
Exotic
Exotic2
Fist
Misc
Dagger
Thrown
Spear
Crossbow
Wand
FishingPole
)
*/
type ItemWeaponSubClass uint

type ItemSubclasses []ItemWeaponSubClass

func (iscs ItemSubclasses) Mask() uint { return GetMask(iscs) }
