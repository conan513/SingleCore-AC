// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

package dbc

import (
	"os"
	"path/filepath"
)

func mkBaseDirs(paths ...string) error {
	for _, p := range paths {
		if err := os.MkdirAll(filepath.Dir(p), 0755); err != nil {
			return err
		}
	}
	return nil
}
