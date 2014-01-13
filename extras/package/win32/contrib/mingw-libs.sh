#!/bin/sh

export PREFIX=/usr 

./test.sh


case $1 in
    --download)
                mkdir -p temp
                cd temp
		wget http://taglib.github.io/releases/taglib-1.9.1.tar.gz
    ;;
    --build)
                cd temp
                tar xvzf taglib-1.9.1.tar.gz
                cd taglib-1.9.1
                cmake ./ -DCMAKE_INSTALL_PREFIX=C:/devel/mingw32-libs -G "MSYS Makefiles"
                mingw32-make -j3
                mingw32-make install
    ;;
        *)
		echo "Commands:"
		echo "--download"
esac
