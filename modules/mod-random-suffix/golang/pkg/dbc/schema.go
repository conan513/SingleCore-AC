// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

package dbc

import (
	"encoding/json"
	"fmt"
	"io"

	"github.com/pkg/errors"
)

//go:generate go run github.com/abice/go-enum --marshal --sql --names --noprefix --prefix DBCSchemaField
/*
ENUM(
int32
uint8
uint32
float32
float64 // float64 is a double value
string_offset
unknown
)
*/
type DBCSchemaFieldType int

// SizeOf returns the sizeof the field in terms of number of bytes
func (t DBCSchemaFieldType) SizeOf() uint32 {
	switch t {
	case DBCSchemaFieldUint8:
		return 1
	case DBCSchemaFieldInt32,
		DBCSchemaFieldUint32,
		DBCSchemaFieldFloat32,
		DBCSchemaFieldStringOffset:
		return 4
	case DBCSchemaFieldFloat64:
		return 8
	}
	log.Panic("Should not reach here at all", "t", t)
	return 0
}

// DBCSchemaField is a DBC schema field definition, it contains a type definition, as well as a name.
type DBCSchemaField struct {
	Type DBCSchemaFieldType `json:"type"`
	Name string             `json:"name"`
}

// the DBCSchema contains the list of schema fields as well as other potentially useful information
type DBCSchema struct {
	Fields []DBCSchemaField `json:"fields"`
}

func (s *DBCSchema) FromJSONReader(schemaRead io.Reader) error {
	err := json.NewDecoder(schemaRead).Decode(&s.Fields)
	if err != nil {
		return errors.Wrapf(err, "unable to read Fields from JSON")
	}
	return nil
}

func (s DBCSchema) FieldName(i int) string {
	colName := s.Fields[i].Name
	if colName == "" {
		colName = fmt.Sprint("field_", i)
	}
	return colName
}

func (s DBCSchema) CalculateRecordSize() uint32 {
	var schemaRecordSize uint32
	for _, f := range s.Fields {
		schemaRecordSize += f.Type.SizeOf()
	}
	return schemaRecordSize
}
