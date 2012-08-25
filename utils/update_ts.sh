#!/bin/sh

for file in `find ../ -name "*.pro"`
do
    ts_pro=`cat $file | grep TRANSLATIONS`
    if [ "$ts_pro" ]
    then
        lupdate -no-obsolete -silent -extensions 'cpp,ui' $file
    fi

done

