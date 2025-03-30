#!/usr/bin/bash

if [[ -d $1 && -d $2 ]]
then
    differ=1

    if [[ -d "$1/assets" && -d "$2/assets" ]]
    then
        diff -r -q "$1/assets" "$2/assets"
        differ=$?
    fi

    if [[ $differ -eq 1 ]]
    then
        echo "Copying assets folder"
        rm -rf "$2/assets/"
        cp -r "$1/assets/" "$2/"
    fi
fi
