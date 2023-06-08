// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

package dbc

import (
	"os"

	"github.com/pkg/errors"
)

func ExportFromCSV(dst, srcCSV, schemaJSON string) error {
	log := log.With("dst", dst, "src_csv", srcCSV, "schema_json", schemaJSON)
	err := mkBaseDirs(dst, srcCSV, schemaJSON)
	defer func() {
		if err != nil {
			log.Error("error encountered on export", "err", err)
		}
	}()
	if err != nil {
		return err
	}
	log.Info("reading source CSV file")
	var srcfp, schemafp, dstfp *os.File
	srcfp, err = os.OpenFile(srcCSV, os.O_RDONLY, 0644)
	if err != nil {
		return errors.Wrap(err, "cannot open source CSV file")
	}
	schemafp, err = os.OpenFile(schemaJSON, os.O_RDONLY, 0644)
	if err != nil {
		return errors.Wrap(err, "cannot open schema file")
	}
	var dbc DBC
	err = dbc.extractFromCSV(srcfp, schemafp)
	if err != nil {
		return err
	}
	dstfp, err = os.OpenFile(dst, os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0644)
	if err != nil {
		return errors.Wrap(err, "cannot open destination CSV file")
	}
	err = dbc.Export(dstfp)
	if err != nil {
		return err
	}
	return nil
}
