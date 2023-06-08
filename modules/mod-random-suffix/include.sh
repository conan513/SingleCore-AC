#!/usr/bin/env bash

MOD_RANDOMENCHANTS_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

source "$MOD_RANDOMENCHANTS_ROOT/conf/conf.sh.dist"

if [ -f "$MOD_RANDOMENCHANTS_ROOT/conf/conf.sh" ]; then
    source "$MOD_RANDOMENCHANTS_ROOT/conf/conf.sh"
fi
