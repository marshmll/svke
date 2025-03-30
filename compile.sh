#!/bin/bash

if [[ -d "$1/assets/shaders" && -d "$2/assets/shaders" ]]
then
    diff -r -q "$1/assets/shaders" "$2/assets/shaders"
    differ=$?

   if [[ $differ -eq 1 ]]
   then
        for i in `find $1/assets/shaders -name "*.vert" -type f`; do
            echo "Compiling $i to $i.spv"
            glslc $i -o $i.spv
        done

        for i in `find $1/assets/shaders -name "*.frag" -type f`; do
            echo "Compiling $i to $i.spv"
            glslc $i -o $i.spv
        done
    fi
fi
