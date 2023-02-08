#!/bin/bash

bison -d -v -y -b parser_sysy --debug -p sysy parser.y
mv parser_sysy.tab.c parser_sysy.tab.cc