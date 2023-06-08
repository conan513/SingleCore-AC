// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

package acoremodrandomsuffix

import (
	_ "embed"
	"reflect"
	"text/template"

	"github.com/lohvht/logi"
	"github.com/lohvht/logi/iface"
)

var log iface.Logger = logi.Get().Named("acoremodrandomsuffix")

var allAttributes Attributes

var tmplFns = template.FuncMap{
	"last": func(x int, a interface{}) bool {
		return x == reflect.ValueOf(a).Len()-1
	},
}

//go:embed generated_sql.tmpl.sql
var generatedSQLTemplateBytes []byte

var generatedSQLTemplate *template.Template

type tmplItemRandomSuffixEntry struct {
	ID              int32
	Name_Lang_enUS  string
	Name_Lang_Mask  int64
	InternalName    string
	Enchantment_1   int32
	Enchantment_2   int32
	Enchantment_3   int32
	Enchantment_4   int32
	Enchantment_5   int32
	AllocationPct_1 int32
	AllocationPct_2 int32
	AllocationPct_3 int32
	AllocationPct_4 int32
	AllocationPct_5 int32
}

type tmplSpellItemEnchantmentEntry struct {
	ID             string
	Effect_1       string
	EffectArg_1    string
	Name_Lang_enUS string
	Name_Lang_Mask string
}

type tmplData struct {
	CustomItemRandomSuffixStartID     int32
	CustomItemRandomSuffixEndID       int32
	CustomSpellItemEnchantmentEntries []tmplSpellItemEnchantmentEntry
	CustomItemRandomSuffixEntries     []tmplItemRandomSuffixEntry
	ItemEnchantmentRandomSuffixes     []customRandomSuffixEntry
}

func init() {
	for _, n := range AttributeNames() {
		a, err := ParseAttribute(n)
		if err != nil {
			log.Panic("Invalid attribute", "err", err.Error(), "attr_name", n)
		}
		allAttributes = append(allAttributes, a)
	}
	generatedSQLTemplate = template.Must(template.New("").Funcs(tmplFns).Parse(string(generatedSQLTemplateBytes)))
}
