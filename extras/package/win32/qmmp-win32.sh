#!/bin/sh

QMMP_VERSION=0.9.3
QMMP_PLUGIN_PACK_VERSION=0.9.1

export DEV_PATH=/c/devel
export MINGW32_PATH=${DEV_PATH}/mingw32
export QT4_PATH=${DEV_PATH}/qt4
export ZLIB_ROOT=${MINGW32_PATH}/i686-w64-mingw32
export PREFIX=${DEV_PATH}/mingw32-libs
export PATH=${PATH}:${MINGW32_PATH}/bin:${QT4_PATH}/bin:${PREFIX}/bin
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig 

export JOBS=2



download_qmmp_tarball()
{
  mkdir -p tmp
  cd tmp
  echo 'downloading qmmp...'
  wget -nc http://qmmp.ylsoftware.com/files/qmmp-${QMMP_VERSION}.tar.bz2
  tar xvjf qmmp-${QMMP_VERSION}.tar.bz2
  cd ..
}

download_plugins_tarball()
{
  mkdir -p tmp
  cd tmp
  echo 'downloading qmmp-plugin-pack...'
  wget -nc http://qmmp.ylsoftware.com/files/plugins/qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}.tar.bz2
  tar xvjf qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}.tar.bz2
  cd ..
}

download_qmmp_svn()
{
  mkdir -p tmp
  cd tmp
  echo 'downloading qmmp...'
  svn checkout svn://svn.code.sf.net/p/qmmp-dev/code/trunk/qmmp qmmp-${QMMP_VERSION}
  cd ..
}

download_plugins_svn()
{
  mkdir -p tmp
  cd tmp
  echo 'downloading qmmp-plugin-pack...'
  svn checkout svn://svn.code.sf.net/p/qmmp-dev/code/trunk/qmmp-plugin-pack qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}
  cd ..
}

build ()
{ 
  cd qmmp-${QMMP_VERSION}
  qmake CONFIG+=release
  mingw32-make -j${JOBS}
  cd ..
  cd qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}
  qmake CONFIG+=release INCLUDEPATH+=`dirs`/../qmmp-${QMMP_VERSION}/src QMAKE_LIBDIR+=`dirs`/../qmmp-${QMMP_VERSION}/bin
  mingw32-make -j${JOBS}
  cd ..
}

create_distr ()
{
  mkdir -p qmmp-distr
  cd qmmp-distr
  mkdir -p translations
  cp -v ../../*.txt ./
  cp -v ../../*.nsi ./
  cp -v ../../*.conf ./
  cp -v ../../unzip.exe ./
  cp -rv ../../themes ./
  cp -rv ../../skins ./
  cp -v ../qmmp-${QMMP_VERSION}/bin/*.exe ./
  cp -v ../qmmp-${QMMP_VERSION}/bin/*.dll ./
  cp -rv ../qmmp-${QMMP_VERSION}/bin/plugins ./
  cp -rv ../qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}/bin/plugins ./
  find . -type f -name *.a -delete
  find . -type d -name ".svn" | xargs rm -rf
  #Qt libs
  for LIB_NAME in QtCore4.dll QtGui4.dll QtNetwork4.dll QtOpenGL4.dll
  do
    cp -v ${QT4_PATH}/bin/${LIB_NAME} ./
  done
  #Qt plugins
  mkdir -p plugins/imageformats plugins/codecs
  for LIB_NAME in qcncodecs4.dll qjpcodecs4.dll qkrcodecs4.dll qtwcodecs4.dll
  do
    cp -v ${QT4_PATH}/plugins/codecs/${LIB_NAME} ./plugins/codecs
  done
  for LIB_NAME in qgif4.dll qico4.dll qjpeg4.dll qsvg4.dll
  do
    cp -v ${QT4_PATH}/plugins/imageformats/${LIB_NAME} ./plugins/imageformats
  done
  #translations
  cp -v ${QT4_PATH}/translations/qt_??.qm ./translations
  #mingw32 libs
  for LIB_NAME in libgcc_s_dw2-1.dll libstdc++-6.dll libwinpthread-1.dll
  do
    cp -v ${MINGW32_PATH}/bin/${LIB_NAME} ./
  done
  #third party libs   
  for LIB_NAME in avcodec-55.dll avformat-55.dll avutil-52.dll glew32.dll libFLAC-8.dll libcddb-2.dll libcdio-13.dll libcdio_cdda-1.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./
  done
  for LIB_NAME in libcurl.dll libenca-0.dll libgme.dll libgnurx-0.dll libmad-0.dll libmodplug-1.dll libmpcdec.dll libogg-0.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./
  done
  for LIB_NAME in libopus-0.dll libopusfile-0.dll libprojectM.dll libsidplayfp-3.dll libsndfile-1.dll libtag.dll libvorbis-0.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./
  done
  for LIB_NAME in libvorbisfile-3.dll libwavpack-1.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./
  done
  #projectM presets
  cp -rv ${PREFIX}/share/projectM/ ./
  #rusxmms
  mkdir -p rusxmms
  for LIB_NAME in libxml2-2.dll librcd-0.dll librcc.dll
  do
    cp -v ${PREFIX}/bin/${LIB_NAME} ./rusxmms
  done
  cp -v	${PREFIX}/taglib-rusxmms/bin/libtag.dll ./rusxmms	     
  cd ..
}


case $1 in
  --download)
    download_qmmp_tarball
    download_plugins_tarball
    #download_qmmp_svn
    #download_plugins_svn
  ;;
  --install)
    cd tmp
    build
    create_distr
    find qmmp-distr -type f -name *.dll   | xargs strip -v
    find qmmp-distr -type f -name *.exe   | xargs strip -v 
  ;;
  --clean)
    cd tmp
    rm -rf qmmp-distr
    rm -rf qmmp-${QMMP_VERSION}
    rm -rf qmmp-plugin-pack-${QMMP_PLUGIN_PACK_VERSION}
  ;;
  --clean-src)
    rm -rf tmp
  ;;
  *)
    echo "Commands:"
    echo "--download"
    echo "--install"
    echo "--clean"
    echo "--clean-src"
  ;;
esac
