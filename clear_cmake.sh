#!/bin/sh


clear () { rm -rf CMakeFiles; rm -f Makefile; rm -f cmake_install.cmake; rm -f CMakeCache.txt; }

make clean
clear
#clear src 
cd src
clear
cd ..
#clear lib
cd lib
clear
#clear qmmp
cd qmmp
clear
#clear Input
cd Input
clear
#clear ffmpeg
cd ffmpeg
clear
#clear mad
cd ..
cd mad
clear
#clear mpc
cd ..
cd mpc 
clear
#clear flac
cd ..
cd flac
clear
#clear vorbis
cd ..
cd vorbis
clear
#clear Output
cd ..
cd ..
cd Output 
clear
#clear alsa
cd alsa 
clear
#clear jack
cd ..
cd jack 
clear
#clear oss
cd ..
cd oss
clear
#clear visual
cd ..
cd ..
cd Visual
clear
cd analyzer
clear
#clear effect
cd ..
cd ..
cd Effect
clear
cd srconverter
clear
