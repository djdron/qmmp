#!/bin/sh

###################################################
# Removes remaining qmake files from qmmp package #
###################################################

make distclean
rm -rf lib
find . -name *.qm | xargs rm
find . -name ".build" | xargs rm -rf

