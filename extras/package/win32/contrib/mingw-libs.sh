#!/bin/sh


LIB_NAMES='taglib libmad'

export PREFIX=C:/devel/mingw32-libs
export STRIP=true


mkdir -p ${PREFIX} ${PREFIX}/bin ${PREFIX}/lib/pkgconfig ${PREFIX}/share/doc
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig 

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
