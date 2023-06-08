// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

package main

import (
	"flag"
	"os"
	"path/filepath"

	"github.com/lohvht/logi"
	"github.com/lohvht/logi/iface"
	"github.com/lohvht/mod-random-suffix/golang/pkg/acoremodrandomsuffix"
)

var log iface.Logger = logi.Get().Named("cmd_dbcappend")

// mkDirAndReadFile will create the dirs up a certain path and then open a file pointer at the path.
func mkDirAndReadFile(path string) *os.File {
	err := os.MkdirAll(filepath.Dir(path), 0755)
	if err != nil {
		log.Panic("cannot open make dir up to path", "path", path, "err", err)
	}
	dstFP, err := os.Open(path)
	if err != nil {
		log.Panic("cannot open destination file", "path", path, "err", err)
	}
	return dstFP
}

func main() {
	cfgPath := flag.String("config", "./generatesuffixes.conf.yaml", "The config file to read generate from")
	flag.Parse()
	cfgFile := mkDirAndReadFile(*cfgPath)
	var cfg acoremodrandomsuffix.Config
	pcfg, err := cfg.ProcessFromReader(cfgFile)
	if err != nil {
		log.Panic("read and process cfg error", "err", err)
	}
	err = acoremodrandomsuffix.Generate(pcfg)
	if err != nil {
		log.Panic("Generate error", "err", err)
	}
}
