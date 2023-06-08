#!/usr/bin/env bash

MOD_WEAPON_VISUAL_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

source $MOD_WEAPON_VISUAL_ROOT"/conf/conf.sh.dist"

if [ -f $MOD_WEAPON_VISUAL_ROOT"/conf/conf.sh" ]; then
    source $MOD_WEAPON_VISUAL_ROOT"/conf/conf.sh"
fi
