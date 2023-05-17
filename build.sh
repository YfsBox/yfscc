#!/bin/bash

make p && make l
rm -r build
mkdir build
cd build
cmake ..

