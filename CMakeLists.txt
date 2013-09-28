# Main CMake file

set(PROJECT_NAME "Repetier-Server")
project(${PROJECT_NAME})
set(Server_VERSION_MAJOR 0)
set(Server_VERSION_MINOR 50)

cmake_minimum_required(VERSION 2.6)
ADD_DEFINITIONS( "-DSERVER_VERSION_MAJOR=${Server_VERSION_MAJOR}" )
ADD_DEFINITIONS( "-DSERVER_VERSION_MINOR=${Server_VERSION_MINOR}" )

# Appends the cmake/modules path inside the MAKE_MODULE_PATH variable which stores the
# directories of additional CMake modules (eg MacroOutOfSourceBuild.cmake):
set(CMAKE_MODULE_PATH ${Repetier-Server_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
 
# The macro below forces the build directory to be different from source directory:
include(MacroOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build.")
include( CheckIncludeFile )
CHECK_INCLUDE_FILE( xlocale.h HAVE_XLOCALE_H )
IF(HAVE_XLOCALE_H)
    ADD_DEFINITIONS("-DHAVE_XLOCALE_H")
ENDIF()
############## include Boost

# Activate debug if you don't know where cmake is looking for your boost files
set(Boost_DEBUG 1) 
SET(BOOST_INCLUDEDIR "C:/Boost/include/boost-1_52")
SET(BOOST_LIBRARYDIR "C:/Boost/lib") 
 
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON) 
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.48.0 COMPONENTS filesystem thread program_options system date_time chrono regex)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
  #  target_link_libraries("Repetier-Server" ${Boost_LIBRARIES})
ENDIF()


add_subdirectory(Repetier-Server)
INCLUDE_DIRECTORIES("Repetier-Server/json_spirit")
INCLUDE_DIRECTORIES("Repetier-Server/moFileReader")
# INCLUDE_DIRECTORIES("Repetier-Server/libconfig")
INCLUDE_DIRECTORIES("Repetier-Server/server")
INCLUDE_DIRECTORIES("Repetier-Server/Poco/Foundation/include")
INCLUDE_DIRECTORIES("Repetier-Server/Poco/Util/include")
INCLUDE_DIRECTORIES("Repetier-Server/Poco/XML/include")
INCLUDE_DIRECTORIES("Repetier-Server/Poco/Net/include")

#message("Files: ${RepetierServer_SOURCES}")
#message("Boost libs: ${Boost_LIBRARIES}")
add_executable("RepetierServer" ${RepetierServer_SOURCES})
target_link_libraries("RepetierServer" ${Boost_LIBRARIES} PocoFoundation PocoNET PocoUtil PocoXML json_spirit mo_file_reader sqlite)
IF (UNIX)
  target_link_libraries(RepetierServer dl m)
ENDIF (UNIX)

install(TARGETS RepetierServer DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" CONFIGURATIONS Debug)
install(DIRECTORY www DESTINATION www)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR/osdependent/osx/repetier-server.xml.in ${CMAKE_CURRENT_BINARY_DIR/etc/repetier-server.xml @ONLY)
    install(FILE ${CMAKE_CURRENT_BINARY_DIR/etc/repetier-server.xml DESTINATION etc/repetier-server.xml)
ENDIF()
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
ENDIF()
if(WIN32)
ENDIF()

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENCE")
set(CPACK_PACKAGE_VERSION_MAJOR "${Server_VERSION_MAJOR")
set(CPACK_PACKAGE_VERSION_MINOR "${Server_VERSION_MINOR")
include(CPack)
