#!/usr/bin/bash

if [[ -d $1 && -d $2 ]]
then
    diff -r -q "$1/assets" "$2/assets"
    differ=$?

    if [[ $differ -eq 1 ]]
    then
        echo "Copying assets folder"
        rm -rf "$2/assets/"
        cp -r "$1/assets/" "$2/"
    fi
fi
