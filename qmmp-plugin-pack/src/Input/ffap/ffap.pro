include(../../plugins.pri)


HEADERS += decoderffapfactory.h ffap.h decoder_ffap.h \
    cueparser.h \
    ffapmetadatamodel.h \
    replaygainreader.h
SOURCES += decoderffapfactory.cpp ffap.c decoder_ffap.cpp \
    cueparser.cpp \
    ffapmetadatamodel.cpp \
    replaygainreader.cpp

ASM_SOURCES += dsputil_yasm.asm x86inc.asm


asm_compiler.commands = yasm
asm_compiler.commands += ${QMAKE_FILE_IN} -f elf -o ${QMAKE_FILE_OUT}
asm_compiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${first(QMAKE_EXT_OBJ)}
asm_compiler.input = ASM_SOURCES
asm_compiler.variable_out = OBJECTS
asm_compiler.name = compiling[asm] ${QMAKE_FILE_IN}
silent:asm_compiler.commands = @echo compiling[asm] ${QMAKE_FILE_IN} && $$asm_compiler.commands
QMAKE_EXTRA_COMPILERS += asm_compiler


contains(QMAKE_CXXFLAGS, -msse2):DEFINES += HAVE_SSE2

#DEFINES += HAVE_SSE2

contains(QMAKE_CXXFLAGS, -m32) {
  message(Architecture is x86)
  DEFINES += ARCH_X86_32
} else:contains(QMAKE_CXXFLAGS, -m64) {
  message(Architecture is x86_64)
  DEFINES += ARCH_X86_64
} else {
  message(Architecture is unknown)
  DEFINES += ARCH_UNKNOWN
}


TARGET = $$PLUGINS_PREFIX/Input/ffap
unix:QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libffap.so


CONFIG += release \
    warn_on \
    plugin \
    link_pkgconfig
TEMPLATE = lib

unix:PKGCONFIG += qmmp taglib

TRANSLATIONS = translations/ffap_plugin_ru.ts \
    translations/ffap_plugin_uk_UA.ts \
    translations/ffap_plugin_zh_CN.ts \
    translations/ffap_plugin_zh_TW.ts \
    translations/ffap_plugin_cs.ts \
    translations/ffap_plugin_pl.ts \
    translations/ffap_plugin_de.ts \
    translations/ffap_plugin_it.ts \
    translations/ffap_plugin_tr.ts \
    translations/ffap_plugin_tr.ts \
    translations/ffap_plugin_lt.ts \
    translations/ffap_plugin_nl.ts \
    translations/ffap_plugin_ja.ts \
    translations/ffap_plugin_es.ts

RESOURCES = translations/translations.qrc
unix {
    isEmpty(LIB_DIR):LIB_DIR = /lib
    target.path = $$LIB_DIR/qmmp/Input
    INSTALLS += target
}
win32:HEADERS += ../../../../src/qmmp/metadatamodel.h \
    ../../../../src/qmmp/decoderfactory.h
