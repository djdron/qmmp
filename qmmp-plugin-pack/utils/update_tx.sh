#!/bin/sh

PROJECT_NAME=qmmp-plugin-pack
TX_CONFIG="../.tx/config"

echo "[main]" > ${TX_CONFIG}
echo "host = https://www.transifex.com" >> ${TX_CONFIG}
echo "" >> ${TX_CONFIG}

for tr_dir in `find ../src/ -type d -name "translations"`
do

     plug_name=`echo $tr_dir | cut -d '/' -f 4`

     if  [ -z "$plug_name" ]; then
         plug_name=`echo $tr_dir | cut -d '/' -f 3`
     fi

     echo "Updating ${plug_name}"

     file_filter=`echo ${tr_dir} | sed 's/..\///'`
     echo "[${PROJECT_NAME}.${plug_name}]" >> ${TX_CONFIG}
     echo "file_filter = ${file_filter}/${plug_name}_plugin_<lang>.ts" >> ${TX_CONFIG}
     echo "source_lang = en" >> ${TX_CONFIG}
     echo "source_file = ${file_filter}/${plug_name}_plugin_en.ts" >> ${TX_CONFIG}
     echo "type = QT" >> ${TX_CONFIG}
     echo "" >> ${TX_CONFIG}
done

for RESOURCE_NAME in qsui_authors
do
    echo "[${PROJECT_NAME}.${RESOURCE_NAME}]" >> ${TX_CONFIG}
    echo "file_filter = src/Ui/qsui/txt/${RESOURCE_NAME}_<lang>.txt" >> ${TX_CONFIG}
    echo "source_lang = en" >> ${TX_CONFIG}
    echo "source_file = src/Ui/qsui/txt/${RESOURCE_NAME}.txt" >> ${TX_CONFIG}
    echo "type = TXT" >> ${TX_CONFIG}
    echo "" >> ${TX_CONFIG}
done
