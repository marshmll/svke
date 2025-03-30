#!/usr/bin/bash

if [[ -d $1 && -d $2 ]] then
    rm -rf "$2/assets/"
    cp -r "$1/assets/" "$2/"
fi
