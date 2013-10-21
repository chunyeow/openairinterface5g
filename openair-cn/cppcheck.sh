#!/bin/sh
cppcheck . --enable=all --xml --verbose --xml-version=2 -j4 2> err.xml
