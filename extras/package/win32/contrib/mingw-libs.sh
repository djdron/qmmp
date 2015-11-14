#!/bin/sh

LIB_NAMES+='pkg-config yasm taglib libmad libogg libvorbis flac wavpack libsndfile opus opusfile '
LIB_NAMES+='libbs2b curl libcdio libgnurx libcddb ffmpeg libmodplug game-music-emu musepack '
LIB_NAMES+='glew projectm xa libsidplayfp enca '
LIB_NAMES+='librcd librcc taglib-rusxmms' #rusxmms

export DEV_PATH=/c/devel
export MINGW32_PATH=${DEV_PATH}/mingw32
export QT4_PATH=${DEV_PATH}/qt4
export ZLIB_ROOT=${MINGW32_PATH}/i686-w64-mingw32
export PREFIX=${DEV_PATH}/mingw32-libs

export PATH=${PATH}:${MINGW32_PATH}/bin:${QT4_PATH}/bin:${PREFIX}/bin

export STRIP=true
export JOBS=2


mkdir -p ${PREFIX} ${PREFIX}/bin ${PREFIX}/lib/pkgconfig ${PREFIX}/share/doc
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig 

case $1 in
  --download)
    for NAME in $LIB_NAMES
    do
        echo 'downloading '${NAME}'...'
        cd $NAME
        sh ./$NAME.sh $1
        cd ..
    done
  ;;
  --install)
    for NAME in $LIB_NAMES
    do
        echo 'installing '${NAME}'...'
        cd $NAME
        sh ./$NAME.sh --clean
        sh ./$NAME.sh --install
        sh ./$NAME.sh --clean
        cd ..
    done
    if [ "$STRIP" == "true" ]; then
        strip -v ${PREFIX}/bin/*.dll
    fi
  ;;
  --clean)
    for NAME in $LIB_NAMES
    do
        echo 'cleaning '${NAME}'...'
        if [ -e $NAME/temp ]; then
          cd $NAME
          sh ./$NAME.sh --clean
          cd ..
        fi
    done

  ;;
  --clean-src)
    for NAME in $LIB_NAMES
    do
        echo 'cleaning '${NAME}'...'
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
