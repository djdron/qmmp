#!/bin/sh



LIB_NAMES+='pkg-config yasm taglib libmad libogg libvorbis flac wavpack libsndfile opus opusfile '
LIB_NAMES+='libbs2b curl libcdio libgnurx libcddb ffmpeg libmodplug'


export PREFIX=D:/devel/mingw32-libs
export STRIP=true
export JOBS=4


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
