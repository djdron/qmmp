include(FindPkgConfig)
pkg_check_modules(QMMPUI qmmpui>=0.6.0)
pkg_check_modules(QMMP qmmp>=0.6.0)
add_subdirectory(Input)
