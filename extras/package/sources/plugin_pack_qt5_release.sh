#!/bin/sh

if [ ! -d "cache" ]; then
mkdir cache
fi


cd cache

echo "Receiving sources.."
if [ ! -d "qmmp-plugin-pack-svn" ]; then
svn checkout svn://svn.code.sf.net/p/qmmp-dev/code/branches/qmmp-plugin-pack-1.0/ qmmp-plugin-pack-svn
fi

cd qmmp-plugin-pack-svn
svn up
echo "Creating changelog.."
svn log > ChangeLog.svn

VERSION=`cat qmmp-plugin-pack.pri | grep "QMMP_PLUGIN_PACK_VERSION" | cut -d " " -f3`

TARBALL=qmmp-plugin-pack-$VERSION


echo Sources name: $TARBALL
cd ..
if [ -d $TARBALL ]; then
echo "Removing previous directory.."
rm -rf $TARBALL
fi

echo "Copying sources.."
cp -r qmmp-plugin-pack-svn $TARBALL

cd $TARBALL

echo "Removing svn tags.."
cd utils
./remove_svn_tags.sh
cd ..

echo "Fixing versions.."
sed 's/SET(SVN_VERSION TRUE)/SET(SVN_VERSION FALSE)/' -i CMakeLists.txt
sed 's/CONFIG += SVN_VERSION/#CONFIG += SVN_VERSION/' -i qmmp-plugin-pack.pri
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
cd files
if [ ! -d "plugins" ]; then
mkdir plugins
fi
cd ..
mv cache/${TARBALL}.tar.bz2 files/plugins/
mv cache/${TARBALL}.tar.bz2.md5sum files/plugins/

echo ""
echo "****** RELEASED FILES *******"
echo "Tarball: ${TARBALL}.tar.bz2"
echo "MD5: ${TARBALL}.tar.bz2.md5sum"
echo "*****************************"
echo ""
echo "Finished"
