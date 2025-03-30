#!/bin/bash

for i in `find assets/shaders -name "*.vert" -type f`; do
    echo "Compiling $i to $i.spv"
    glslc $i -o $i.spv
done

for i in `find assets/shaders -name "*.frag" -type f`; do
    echo "Compiling $i to $i.spv"
    glslc $i -o $i.spv
done
