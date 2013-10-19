# Main CMake file

set(PROJECT_NAME "Repetier-Server")
set(PROJECT_DIR "Repetier-Server")
set(PROJECT_TARGET RepetierServer)
set(SERVER_PORT 3344)

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
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)
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
INCLUDE_DIRECTORIES("Repetier-Server")
INCLUDE_DIRECTORIES("Repetier-Server/json_spirit")
INCLUDE_DIRECTORIES("Repetier-Server/moFileReader")
# INCLUDE_DIRECTORIES("Repetier-Server/libconfig")
INCLUDE_DIRECTORIES("Repetier-Server/server")
INCLUDE_DIRECTORIES("Repetier-Server/Poco/Foundation/include")
INCLUDE_DIRECTORIES("Repetier-Server/Poco/Util/include")
INCLUDE_DIRECTORIES("Repetier-Server/Poco/XML/include")
INCLUDE_DIRECTORIES("Repetier-Server/Poco/Net/include")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Repetier-Server/productData.h.in ${CMAKE_CURRENT_SOURCE_DIR}/Repetier-Server/productData.h)

#message("Files: ${RepetierServer_SOURCES}")
#message("Boost libs: ${Boost_LIBRARIES}")
add_executable(${PROJECT_TARGET} ${RepetierServer_SOURCES})
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(INSTALL_PATH /usr/local/${PROJECT_DIR})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/osdependent/osx/repetier-server.xml.in ${CMAKE_CURRENT_BINARY_DIR}/etc/${PROJECT_TARGET}.xml)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/etc/repetier-server.xml DESTINATION ${INSTALL_PATH}/etc)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/osdependent/osx/launchd.plist.in ${CMAKE_CURRENT_BINARY_DIR}/etc/${PROJECT_TARGET}.plist)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/etc/${PROJECT_TARGET}.plist DESTINATION ${INSTALL_PATH}/etc)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/osdependent/osx/installLaunchd.sh.in ${CMAKE_CURRENT_BINARY_DIR}/bin/installLaunchd.sh @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bin/installLaunchd.sh DESTINATION ${INSTALL_PATH}/bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE WORLD_READ WORLD_EXECUTE)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/osdependent/osx/uninstallLaunchd.sh.in ${CMAKE_CURRENT_BINARY_DIR}/bin/uninstallLaunchd.sh @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bin/uninstallLaunchd.sh DESTINATION ${INSTALL_PATH}/bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE WORLD_READ WORLD_EXECUTE)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/osdependent/osx/createpackage.sh.in ${CMAKE_CURRENT_BINARY_DIR}/bin/createpackage.sh @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/osdependent/osx/scripts/postinstall.in ${CMAKE_CURRENT_SOURCE_DIR}/osdependent/osx/scripts/postinstall @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/osdependent/osx/readme.html ${CMAKE_CURRENT_BINARY_DIR}/readme.html @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/osdependent/osx/Distribution.xml ${CMAKE_CURRENT_BINARY_DIR}/Distribution.xml @ONLY)
ENDIF()
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
ENDIF()
if(WIN32)
ENDIF()
install(DIRECTORY www DESTINATION ${INSTALL_PATH})
install(DIRECTORY languages DESTINATION ${INSTALL_PATH})
target_link_libraries(${PROJECT_TARGET} ${Boost_LIBRARIES} PocoFoundation PocoNET PocoUtil PocoXML json_spirit mo_file_reader sqlite)
IF (UNIX)
  target_link_libraries(${PROJECT_TARGET} dl m)
ENDIF (UNIX)
install(TARGETS ${PROJECT_TARGET} DESTINATION "${INSTALL_PATH}/bin")

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${Server_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Server_VERSION_MINOR}")
include(CPack)
