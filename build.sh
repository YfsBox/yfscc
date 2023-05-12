#!/bin/bash


gcc -m32 -c sylib.c -o sylib.o
ar rcs libsysy.a
ar rcs libsysy.a sylib.o

make p && make l
rm -r build
mkdir build
cd build
cmake ..

