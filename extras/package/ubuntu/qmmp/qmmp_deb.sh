#!/bin/sh

QMMP_VERSION=0.9.4
UBUNTU_CODENAMES='precise trusty vivid wily'
BUILD_ROOT=build-root


prepare ()
{
	cp ../qmmp-$QMMP_VERSION.tar.bz2 ./
	bzip2 -dv ./qmmp-$QMMP_VERSION.tar.bz2
    mv ./qmmp-$QMMP_VERSION.tar ./qmmp_$QMMP_VERSION.orig.tar
    gzip ./qmmp_$QMMP_VERSION.orig.tar
}

build ()
{
    echo '****'$1'****'
    mkdir $1
    cd $1
    tar xvjf ../../qmmp-$QMMP_VERSION.tar.bz2
    mkdir qmmp-$QMMP_VERSION/debian
    cp -rv ../../debian-$1/* -t qmmp-$QMMP_VERSION/debian/
    cp ../qmmp_$QMMP_VERSION.orig.tar.gz ./
    cd qmmp-$QMMP_VERSION
    if [ "$1" = "precise" ]; then
        debuild -S -sa -kF594F6B4
    else
	debuild -S -sd -kF594F6B4
    fi
    cd ..
    cd ..
}

update ()
{
    dch -m --newversion ${QMMP_VERSION}-1ubuntu1~${1}0 -D ${1} -c debian-$1/changelog "New upstream release."
}

upload ()
{
	dput ppa:forkotov02/ppa $1/*.changes
}

clean ()
{
    rm -rf $BUILD_ROOT
    rm qmmp-$QMMP_VERSION.tar.bz2
}

case $1 in
    --download)
		wget http://qmmp.ylsoftware.com/files/qmmp-$QMMP_VERSION.tar.bz2
    ;;
    --update)
		for CODENAME in $UBUNTU_CODENAMES
		do
			update $CODENAME
		done
    ;;
    --build)
		rm -rf $BUILD_ROOT
		mkdir $BUILD_ROOT
		cd $BUILD_ROOT
		prepare
		for CODENAME in $UBUNTU_CODENAMES
		do
			build $CODENAME
		done
    ;;
    --upload)
       cd $BUILD_ROOT
       for CODENAME in $UBUNTU_CODENAMES
		do
			upload $CODENAME
		done
    ;;
    --clean)
		clean
    ;;
    *)
		echo "Commands:"
		echo "--download"
		echo "--update"
		echo "--build"
		echo "--upload"
		echo "--clean"
esac
