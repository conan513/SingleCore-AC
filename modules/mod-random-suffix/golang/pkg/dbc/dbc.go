// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

package dbc

import (
	"bytes"
	"encoding/binary"
	"encoding/csv"
	"fmt"
	"io"
	"sort"
	"strconv"

	"github.com/pkg/errors"
)

const (
	dbcMagicSignature         = "WDBC"
	dbcMagicSignatureUint32   = uint32(1128416343)
	validDBCRecordStartOffset = 20
)

var (
	ErrDBCInvalidRecordStartOffset = errors.New("DBC record offset start not correct")
	ErrDBCInvalidSignature         = fmt.Errorf("DBC has an invalid signature, does not begin with '%s' or have a uint32 value of '%d'", dbcMagicSignature, dbcMagicSignatureUint32)
	ErrDBCInvalidFieldCount        = errors.New("The schema / CSV does not tally with the DBC field count passed in, check the schema/CSV again")
	ErrDBCInvalidRecordCount       = errors.New("DBC record size does not tally with the schema passed in, check the schema again")
	ErrDBCInvalidDataStringOffset  = errors.New("DBC string is not a string or bytes")
	ErrDBCInvalidStringBlockOffset = errors.New("DBC string block offsets are wrong")
	ErrDBCInvalidFieldName         = errors.New("DBC Schema's field names do not match the CSV being read in")
)

func init() {
	// sanity check, we're ensuring that our magic signature is actually correctly defined inside our package
	// just in case.
	d := DBCHeader{
		MagicSignature: []byte(dbcMagicSignature),
	}

	if err := d.validateMagicSignature(); err != nil {
		log.Panic("Default DBC magic signature does not match somehow, programmer error?", "err", err)
	}
}

type DBCHeader struct {
	MagicSignature  []byte
	RecordCount     uint32
	FieldCount      uint32
	RecordSize      uint32
	StringBlockSize uint32
}

func (d DBCHeader) validateMagicSignature() error {
	if (string(d.MagicSignature) != dbcMagicSignature) || (binary.LittleEndian.Uint32(d.MagicSignature) != dbcMagicSignatureUint32) {
		return errors.Wrapf(ErrDBCInvalidSignature, "magic signature was: %s", d.MagicSignature)
	}
	return nil
}

func (d *DBCHeader) ReadDBCHeader(dbcRead io.Reader) (err error) {
	d.MagicSignature = make([]byte, 4)
	n, err := dbcRead.Read(d.MagicSignature)
	if err != nil {
		return errors.Wrap(err, "unable to read DBC magic signature for DBC header")
	}
	if err = d.validateMagicSignature(); err != nil {
		return err
	}
	err = binary.Read(dbcRead, binary.LittleEndian, &d.RecordCount)
	if err != nil {
		return errors.Wrap(err, "unable to read DBC header recordCount")
	}
	n += 4 // uint32 read 4 bytes
	err = binary.Read(dbcRead, binary.LittleEndian, &d.FieldCount)
	if err != nil {
		return errors.Wrap(err, "unable to read DBC header fieldCount")
	}
	n += 4 // uint32 read 4 bytes
	err = binary.Read(dbcRead, binary.LittleEndian, &d.RecordSize)
	if err != nil {
		return errors.Wrap(err, "unable to read DBC header recordSize")
	}
	n += 4 // uint32 read 4 bytes
	err = binary.Read(dbcRead, binary.LittleEndian, &d.StringBlockSize)
	if err != nil {
		return errors.Wrap(err, "unable to read DBC header stringBlockSize")
	}
	n += 4 // uint32 read 4 bytes
	if n != validDBCRecordStartOffset {
		return errors.Wrapf(ErrDBCInvalidRecordStartOffset, "recordStartOffset is %d, valid is %d", n, validDBCRecordStartOffset)
	}
	return nil
}

func (d *DBCHeader) WriteDBCHeader(dbcWrite io.Writer) error {
	_, err := dbcWrite.Write(d.MagicSignature)
	if err != nil {
		return errors.Wrap(err, "unable to write DBC header magic signature to the dbc file")
	}
	err = binary.Write(dbcWrite, binary.LittleEndian, d.RecordCount)
	if err != nil {
		return errors.Wrap(err, "unable to write DBC header RecordCount")
	}
	err = binary.Write(dbcWrite, binary.LittleEndian, d.FieldCount)
	if err != nil {
		return errors.Wrap(err, "unable to write DBC header FieldCount")
	}
	err = binary.Write(dbcWrite, binary.LittleEndian, d.RecordSize)
	if err != nil {
		return errors.Wrap(err, "unable to write DBC header RecordSize")
	}
	err = binary.Write(dbcWrite, binary.LittleEndian, d.StringBlockSize)
	if err != nil {
		return errors.Wrap(err, "unable to write DBC header StringBlockSize")
	}
	return nil
}

type DBC struct {
	Header      DBCHeader
	Schema      DBCSchema
	Data        [][]interface{}
	StringBlock map[int]string
}

// validate validates the following:
//  1. the header against the schema, its used as way to check if the header
//     values (from the DBC file) and the Schema values (from the schema.json,
//     or other sources) tallies
func (d DBC) validate() error {
	if len(d.Schema.Fields) != int(d.Header.FieldCount) {
		return errors.Wrapf(ErrDBCInvalidFieldCount, "dbc field count was %d, schema was %d", d.Header.FieldCount, len(d.Schema.Fields))
	}
	schemaRecordSize := d.Schema.CalculateRecordSize()
	if d.Header.RecordSize != schemaRecordSize {
		return errors.Wrapf(ErrDBCInvalidRecordCount, "dbc record size was %d, schema was %d", d.Header.RecordSize, schemaRecordSize)
	}
	return nil
}

// extractStrings extracts the strings from the given ReadSeeker. It assumes that the header for the DBC has already
// been read as it uses the stringblocksize and recordstartoffset from the header to determine where the seeker should
// jump to.
// Returns a map of string offsets from the start of the string block to their respective strings
func (d DBC) extractStrings(rs io.ReadSeeker) (map[int]string, error) {
	m := make(map[int]string)
	var stringBlock []byte
	_, err := rs.Seek(-1*int64(d.Header.StringBlockSize), io.SeekEnd)
	if err != nil {
		return m, errors.Wrap(err, "error seeking to string block")
	}
	stringBlock, err = io.ReadAll(rs)
	if err != nil {
		return m, errors.Wrap(err, "error reading string block")
	}
	var strOffset int
	for _, sB := range bytes.Split(stringBlock, []byte("\u0000")) {
		m[strOffset] = string(sB)
		strOffset += len(sB) + 1
	}
	// log.Info("EXTRACTED OFFSETS", "map", m)
	return m, nil
}

func (d DBC) extractData(rs io.ReadSeeker) (data [][]interface{}, strs map[int]string, err error) {
	strOffsetsToStr, err := d.extractStrings(rs)
	if err != nil {
		return nil, strOffsetsToStr, err
	}
	_, err = rs.Seek(validDBCRecordStartOffset, io.SeekStart)
	if err != nil {
		return nil, strOffsetsToStr, errors.Wrap(err, "unable to backtrack to record start offset")
	}
	for i := 0; i < int(d.Header.RecordCount); i++ {
		var row []interface{}
		for fi, f := range d.Schema.Fields {
			var v interface{}
			switch f.Type {
			case DBCSchemaFieldInt32:
				var i int32
				err = binary.Read(rs, binary.LittleEndian, &i)
				v = i
			case DBCSchemaFieldUint8:
				var i uint8
				err = binary.Read(rs, binary.LittleEndian, &i)
				v = i
			case DBCSchemaFieldUint32:
				var i uint32
				err = binary.Read(rs, binary.LittleEndian, &i)
				v = i
			case DBCSchemaFieldFloat32:
				var i float32
				err = binary.Read(rs, binary.LittleEndian, &i)
				v = i
			case DBCSchemaFieldFloat64:
				var i float64
				err = binary.Read(rs, binary.LittleEndian, &i)
				v = i
			case DBCSchemaFieldStringOffset:
				var i int32
				err = binary.Read(rs, binary.LittleEndian, &i)
				v = strOffsetsToStr[int(i)]
			case DBCSchemaFieldUnknown:
				var i int32
				err = binary.Read(rs, binary.LittleEndian, &i)
				v = i
			default:
				log.Panic("unsupported DBC Schema, check the schema again!", "field_type", f.Type, "col", d.Schema.FieldName(fi))
			}
			if err != nil {
				return nil, strOffsetsToStr, errors.Wrapf(err, "unable to read field in DBC: field_index %d, field_name %s, field_type %s", fi, d.Schema.FieldName(fi), f.Type)
			}
			row = append(row, v)
		}
		data = append(data, row)
	}
	return data, strOffsetsToStr, nil
}

func (d *DBC) extract(dbcRs, schemaRs io.ReadSeeker) error {
	err := d.Header.ReadDBCHeader(dbcRs)
	if err != nil {
		return err
	}
	err = d.Schema.FromJSONReader(schemaRs)
	if err != nil {
		return err
	}
	err = d.validate()
	if err != nil {
		return err
	}
	data, strs, err := d.extractData(dbcRs)
	if err != nil {
		return err
	}
	d.Data = data
	d.StringBlock = strs
	return nil
}

func (d *DBC) stringBlockOffsets() map[string]int {
	m := make(map[string]int)
	for offset, str := range d.StringBlock {
		m[str] = offset
	}
	return m
}

func (d *DBC) Export(w io.WriteSeeker) error {
	err := d.Header.WriteDBCHeader(w)
	if err != nil {
		return err
	}
	stringsToOffsets := d.stringBlockOffsets()
	for _, row := range d.Data {
		for fi, fv := range row {
			schemaField := d.Schema.Fields[fi]
			switch schemaField.Type {
			case DBCSchemaFieldInt32, DBCSchemaFieldUint8, DBCSchemaFieldUint32, DBCSchemaFieldFloat32, DBCSchemaFieldFloat64, DBCSchemaFieldUnknown:
				err = binary.Write(w, binary.LittleEndian, fv)
				if err != nil {
					return errors.Wrapf(err, "unable to write field in DBC: field_index %d, field_name %s, field_type %s, value: %v", fi, d.Schema.FieldName(fi), schemaField.Type, fv)
				}
			case DBCSchemaFieldStringOffset:
				var fvStr string
				switch fvt := fv.(type) {
				case []byte:
					fvStr = string(fvt)
				case string:
					fvStr = fvt
				default:
					return errors.Wrapf(ErrDBCInvalidDataStringOffset, "string offset type was %T, val: %v", fv, fv)
				}
				strOffset := int32(stringsToOffsets[fvStr])
				err = binary.Write(w, binary.LittleEndian, strOffset)
				if err != nil {
					return errors.Wrapf(err, "unable to write field in DBC: field_index %d, field_name %s, field_type %s, value: %v", fi, d.Schema.FieldName(fi), schemaField.Type, fv)
				}
			default:
				log.Panic("unsupported DBC Schema, check the schema again!", "field_type", schemaField.Type, "col", d.Schema.FieldName(fi))
			}
		}
	}
	type strBlockIndex struct {
		offset int
		sB     []byte
	}
	var sbis []strBlockIndex
	for off, s := range d.StringBlock {
		sbis = append(sbis, strBlockIndex{offset: off, sB: []byte(s)})
	}
	sort.SliceStable(sbis, func(i, j int) bool { return sbis[i].offset < sbis[j].offset })
	var strBs [][]byte
	prev := -1
	for i, sbi := range sbis {
		if i == 0 && sbi.offset != 0 && string(sbi.sB) != "" {
			return errors.Wrap(ErrDBCInvalidStringBlockOffset, "first offset should always be 0 and be an empty string")
		}
		if prev+1 != sbi.offset {
			return errors.Wrapf(ErrDBCInvalidStringBlockOffset, "previous offset should always be one less than current, prev %d, current %d", prev, sbi.offset)
		}
		strBs = append(strBs, sbi.sB)
		prev += len(sbi.sB) + 1
	}
	strByteBlock := bytes.Join(strBs, []byte("\u0000"))
	_, err = w.Write(strByteBlock)
	if err != nil {
		return errors.Wrap(err, "unable to write string block to the end of the writer")
	}
	return nil
}

func (d *DBC) ExportToCSV(w io.WriteSeeker) error {
	dstcsvfp := csv.NewWriter(w)
	defer dstcsvfp.Flush()
	var csvHeader []string
	for fi := range d.Schema.Fields {
		csvHeader = append(csvHeader, d.Schema.FieldName(fi))
	}
	err := dstcsvfp.Write(csvHeader)
	if err != nil {
		return errors.Wrap(err, "cannot write CSV header")
	}
	for _, d := range d.Data {
		var dr []string
		for _, dv := range d {
			dr = append(dr, fmt.Sprint(dv))
		}
		err = dstcsvfp.Write(dr)
		if err != nil {
			return errors.Wrap(err, "cannot write data row")
		}
	}
	return nil
}

// AppendFromCSV appends data from a source srcRs. It assumes that srcRs is
// a CSV source.
func (d *DBC) AppendFromCSV(srcRs io.ReadSeeker) error {
	srcCSVR := csv.NewReader(srcRs)
	csvRecords, err := srcCSVR.ReadAll()
	if err != nil {
		return errors.Wrap(err, "unable to read in all CSV records from source CSV")
	}
	return d.AppendFromCSVData(csvRecords)
}

func (d *DBC) AppendFromCSVData(csvRecords [][]string) error {
	csvHeader, csvRecords := csvRecords[0], csvRecords[1:]
	if d.Header.FieldCount != uint32(len(csvHeader)) {
		return errors.Wrapf(ErrDBCInvalidFieldCount, "field count from CSV and field count from header are these respectively: %d, %d", len(csvHeader), d.Header.FieldCount)
	}
	d.Header.RecordCount += uint32(len(csvRecords))

	// // Remove last empty string from string block, will be added back later
	for off, s := range d.StringBlock {
		if s == "" && off != 0 {
			delete(d.StringBlock, off)
		}
	}
	var err error
	stringsToOffsets := d.stringBlockOffsets()
	for _, row := range csvRecords {
		if d.Header.FieldCount != uint32(len(row)) {
			return errors.Wrapf(ErrDBCInvalidFieldCount, "field count from CSV row and field count from header are these respectively: %d, %d", len(row), d.Header.FieldCount)
		}

		var dbcRow []interface{}
		for fi, fieldValue := range row {
			sf := d.Schema.Fields[fi]
			if sf.Name != csvHeader[fi] {
				return errors.Wrapf(ErrDBCInvalidFieldName, "field names do not tally from schema and CSV, schema is %s, csv header is %s", sf.Name, csvHeader[fi])
			}
			var v interface{}
			switch sf.Type {
			case DBCSchemaFieldInt32:
				var i int64
				i, err = strconv.ParseInt(fieldValue, 10, 32)
				v = int32(i)
			case DBCSchemaFieldUint8:
				var i uint64
				i, err = strconv.ParseUint(fieldValue, 10, 8)
				v = uint8(i)
			case DBCSchemaFieldUint32:
				var i uint64
				i, err = strconv.ParseUint(fieldValue, 10, 32)
				v = uint32(i)
			case DBCSchemaFieldFloat32:
				var i float64
				i, err = strconv.ParseFloat(fieldValue, 32)
				v = float32(i)
			case DBCSchemaFieldFloat64:
				var i float64
				i, err = strconv.ParseFloat(fieldValue, 32)
				v = i
			case DBCSchemaFieldStringOffset:
				v = fieldValue
				if _, ok := stringsToOffsets[fieldValue]; len(fieldValue) > 0 && !ok {
					stringsToOffsets[fieldValue] = int(d.Header.StringBlockSize)
					d.StringBlock[int(d.Header.StringBlockSize)] = fieldValue
					// advance offset by the length of bytes and 1, where 1 is the \0 byte
					d.Header.StringBlockSize += uint32(len([]byte(fieldValue))) + 1
				}
			case DBCSchemaFieldUnknown:
				var i int64
				i, err = strconv.ParseInt(fieldValue, 10, 32)
				v = int32(i)
			default:
				log.Panic("unsupported DBC Schema, check the schema again!", "field_type", sf.Type, "col", d.Schema.FieldName(fi))
			}
			if err != nil {
				return errors.Wrapf(err, "unable to read field in CSV: field_index %d, field_name %s, field_type %s", fi, d.Schema.FieldName(fi), sf.Type)
			}
			dbcRow = append(dbcRow, v)
		}
		d.Data = append(d.Data, dbcRow)
	}
	d.StringBlock[int(d.Header.StringBlockSize)] = "" // Last entry also ends off with a control char, we add this in to be consistent.
	return nil
}

// extractFromCSV extracts from a source srcRs. It assumes that srcRs is
// a CSV source.
func (d *DBC) extractFromCSV(srcRs, schemaRs io.ReadSeeker) error {
	err := d.Schema.FromJSONReader(schemaRs)
	if err != nil {
		return err
	}
	d.Header.MagicSignature = []byte(dbcMagicSignature)
	// for our convenience, we just calc the record size according to schema
	// since we don't store type info inside our CSV
	d.Header.RecordSize = d.Schema.CalculateRecordSize()
	d.Header.FieldCount = uint32(len(d.Schema.Fields))
	// Storing initial offsets for our string block
	d.Header.StringBlockSize = 1
	d.StringBlock = map[int]string{
		0: "", // First is always zero and empty string, as the first byte is hardcoded as \0
	}
	return d.AppendFromCSV(srcRs)
}
