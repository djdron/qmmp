#Hack for MacOS X to run fix_mac_libs.sh script after installation
cmake_minimum_required(VERSION 2.8.6)
install(CODE "execute_process(COMMAND ${CMAKE_SOURCE_DIR}/utils/fix_mac_libs.sh ${CMAKE_INSTALL_PREFIX}/qmmp.app)")
