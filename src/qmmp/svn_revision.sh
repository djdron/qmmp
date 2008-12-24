#!/bin/sh

# based on ffmpeg version.sh script
# check for SVN revision number
MYDIR=$(dirname $0)
revision=`cd $MYDIR && LC_ALL=C svn info 2> /dev/null | grep Revision | cut -d' ' -f2`
test $revision || revision=`cd "$1" && grep revision .svn/entries 2>/dev/null | cut -d '"' -f2`
test $revision || revision=`cd "$1" && sed -n -e '/^dir$/{n;p;q}' .svn/entries 2>/dev/null`
test $revision && revision=svn-r$revision

# no version number found
test $revision || revision=svn

echo $revision
