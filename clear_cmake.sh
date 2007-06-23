#!/bin/sh


clean () { rm -rf CMakeFiles; rm -f Makefile; rm -f cmake_install.cmake; rm -f CMakeCache.txt; }

make clean
clean
#clear src 
cd src
clean
cd ..
#clear lib
cd lib
clean
#clear qmmp
cd qmmp
clean
#clear Input
cd Input
clean
#clear ffmpeg
cd ffmpeg
clean
#clear mad
cd ..
cd mad
clean
#clear mpc
cd ..
cd mpc 
clean
#clear flac
cd ..
cd flac
clean
#clear vorbis
cd ..
cd vorbis
clean
#clear Output
cd ..
cd ..
cd Output 
clean
#clear alsa
cd alsa 
clean
#clear jack
cd ..
cd jack 
clean
