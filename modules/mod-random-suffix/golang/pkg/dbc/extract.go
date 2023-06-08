// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

package dbc

import (
	"os"

	"github.com/pkg/errors"
)

func NewDBCFromFile(dbcFilePath, schemaJSONPath string) (DBC, error) {
	log := log.With("dbc_path", dbcFilePath, "schema_path", schemaJSONPath)
	var err error
	defer func() {
		if err != nil {
			log.Error("error encountered on extraction", "err", err)
		}
	}()
	var d DBC
	var dbcfp, schemafp *os.File
	log.Info("reading DBC file from filepath")
	dbcfp, err = os.OpenFile(dbcFilePath, os.O_RDONLY, 0644)
	if err != nil {
		return d, errors.Wrap(err, "cannot open dbc file")
	}
	schemafp, err = os.OpenFile(schemaJSONPath, os.O_RDONLY, 0644)
	if err != nil {
		return d, errors.Wrap(err, "cannot open schema file")
	}
	err = d.extract(dbcfp, schemafp)
	if err != nil {
		return d, err
	}
	return d, nil
}

// ExtractToCSV extracts DBC files into CSV file paths
func ExtractToCSV(dbcFilePath, destCSVPath, schemaPath string) error {
	log := log.With("dbc_path", dbcFilePath, "dst_path", destCSVPath, "schema_path", schemaPath)
	// Make the containing directories if needed.
	err := mkBaseDirs(dbcFilePath, destCSVPath, schemaPath)
	if err != nil {
		return err
	}
	defer func() {
		if err != nil {
			log.Error("error encountered on extraction", "err", err)
		}
	}()
	var dbc DBC
	dbc, err = NewDBCFromFile(dbcFilePath, schemaPath)
	if err != nil {
		return err
	}
	var dstfp *os.File
	dstfp, err = os.OpenFile(destCSVPath, os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0644)
	if err != nil {
		return errors.Wrap(err, "cannot open destination CSV file")
	}
	return dbc.ExportToCSV(dstfp)
}
