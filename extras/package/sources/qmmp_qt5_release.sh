#!/bin/sh

if [ ! -d "cache" ]; then
mkdir cache
fi


cd cache

echo "Receiving sources.."
if [ ! -d "qmmp-qt5-svn" ]; then
svn checkout svn://svn.code.sf.net/p/qmmp-dev/code/branches/qmmp-1.0/ qmmp-qt5-svn
fi

cd qmmp-qt5-svn
svn up
echo "Creating changelog.."
svn log > ChangeLog.svn

MAJOR=`cat src/qmmp/qmmp.h | grep "#define QMMP_VERSION_MAJOR" | cut -d " " -f3`
MINOR=`cat src/qmmp/qmmp.h | grep "#define QMMP_VERSION_MINOR" | cut -d " " -f3`
PATCH=`cat src/qmmp/qmmp.h | grep "#define QMMP_VERSION_PATCH" | cut -d " " -f3`

TARBALL=qmmp-$MAJOR.$MINOR.$PATCH


echo Sources name: $TARBALL
cd ..
if [ -d $TARBALL ]; then
echo "Removing previous directory.."
rm -rf $TARBALL
fi

echo "Copying sources.."
cp -r qmmp-qt5-svn $TARBALL

cd $TARBALL

echo "Removing svn tags.."
cd utils
./remove_svn_tags.sh
cd ..

echo "Fixing version.."
sed 's/QMMP_VERSION_STABLE 0/QMMP_VERSION_STABLE 1/' -i src/qmmp/qmmp.h
cd ..

echo "Creating tarball.."
tar vcjf ${TARBALL}.tar.bz2 ${TARBALL}/

echo "Removing temporary directory.."
rm -rf ${TARBALL}

echo "Creating md5 sum.."
md5sum -b ${TARBALL}.tar.bz2 > ${TARBALL}.tar.bz2.md5sum

echo "Moving released files.."
cd ..
if [ ! -d "files" ]; then
mkdir files
fi
mv cache/${TARBALL}.tar.bz2 files/
mv cache/${TARBALL}.tar.bz2.md5sum files/

echo ""
echo "****** RELEASED FILES *******"
echo "Tarball: ${TARBALL}.tar.bz2"
echo "MD5: ${TARBALL}.tar.bz2.md5sum"
echo "*****************************"
echo ""
echo "Finished"
