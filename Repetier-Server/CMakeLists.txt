# toplevel src CMake file

set(CMAKE_INCLUDE_CURRENT_DIR ON)


add_subdirectory(json_spirit)
# add_subdirectory(libconfig)
add_subdirectory(server)
add_subdirectory(moFileReader)
add_subdirectory(Poco)
add_subdirectory(sqlite)

set(RepetierServer_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" ${SERVER_SOURCES} PARENT_SCOPE)
