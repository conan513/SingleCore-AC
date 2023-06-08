// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

package acoremodrandomsuffix

import (
	"os"
	"path/filepath"

	"github.com/pkg/errors"
	"golang.org/x/exp/constraints"
)

func GetMask[T constraints.Integer](is []T) uint {
	var res uint
	for _, i := range is {
		var shift uint = 1 << i
		if res&shift > 0 {
			log.Panic("value is already within the resultant mask", "a", uint(i), "a_string", i, "mask", res)
		}
		res |= shift
	}
	return res
}

// mkDirAndOpenFile will create the dirs up a certain path and then open a file pointer at the path.
func mkDirAndOpenFile(path string) (*os.File, error) {
	err := os.MkdirAll(filepath.Dir(path), 0755)
	if err != nil {
		return nil, errors.Wrapf(err, "cannot open make dir up to path %s", path)
	}
	dstFP, err := os.OpenFile(path, os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0644)
	if err != nil {
		return nil, errors.Wrapf(err, "cannot open destination file %s", path)
	}
	return dstFP, nil
}
