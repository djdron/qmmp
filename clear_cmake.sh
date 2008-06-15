#!/bin/sh

make clean

rm install_manifest.txt

find . -name *.qm | xargs rm

rm_dir='CMakeFiles'

clear () { rm -f Makefile; rm -f cmake_install.cmake; rm -f CMakeCache.txt; }

walk_dirs(){

    for file in `ls -a`
    do
        if [ -d $file ]
        then
            if [ $file == $rm_dir ]
            then
                echo "Removing $rm_dir directory..."
                rm -rf $rm_dir
                clear
            elif [[ $file != '.' && $file != '..' ]]
            then
                echo "Entering $file"
                cd $file;
                walk_dirs;
                cd ..
            fi
        fi
    done
}

walk_dirs


