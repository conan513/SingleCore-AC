#!/usr/bin/env bash

MOD_AUTO_RESURRECT_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

source $MOD_AUTO_RESURRECT_ROOT"/conf/conf.sh.dist"

if [ -f $MOD_AUTO_RESURRECT_ROOT"/conf/conf.sh" ]; then
    source $MOD_AUTO_RESURRECT_ROOT"/conf/conf.sh"
fi
