SET(USE_QMMP_DIALOG TRUE CACHE BOOL "enable/disable Qmmp file dialog")

IF(USE_QMMP_DIALOG)
add_subdirectory(QmmpFileDialog)
ENDIF(USE_QMMP_DIALOG)
