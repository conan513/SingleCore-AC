#!/usr/bin/env bash
MOD_ACCOUNTBOUND_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

source $MOD_ACCOUNTBOUND_ROOT"/conf/conf.sh.dist"

if [ -f $MOD_ACCOUNTBOUND_ROOT"/conf/conf.sh" ]; then
    source $MOD_ACCOUNTBOUND_ROOT"/conf/conf.sh"
fi
