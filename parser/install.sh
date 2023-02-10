#!/bin/bash

echo "generate parser file"
bison -d -o parser.cpp --debug parser.y