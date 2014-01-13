#!/bin/sh

export PREFIX=D:/devel/mingw32-libs

#LIB_NAMES='taglib libmad'
LIB_NAMES='autoconf'
#LIB_NAMES='m4'

case $1 in
  --download | --install | --clean)
    for NAME in $LIB_NAMES
    do
        cd $NAME
        if [ "$1" = "--install" ];  then
          sh ./$NAME.sh --clean
        fi
        sh ./$NAME.sh $1
    cd ..
    done
  ;;
  --clean-src)
    for NAME in $LIB_NAMES
    do
        cd $NAME
        rm -rf temp
        cd ..
    done
  ;;
  *)
    echo "Commands:"
    echo "--download"
    echo "--install"
    echo "--clean"
    echo "--clean-src"
esac
