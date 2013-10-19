message("Poco/Util")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB UTIL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Poco/Util/*.h)
# Remove windows dependent files - don't use registry anyway.
if(NOT WIN32)
message("Remove windows sources")
file(GLOB WIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/Win*.cpp)
list(REMOVE_ITEM UTIL_SOURCES ${WIN_SOURCES})
endif()

INCLUDE_DIRECTORIES("include")
INCLUDE_DIRECTORIES("../Foundation/include")
INCLUDE_DIRECTORIES("../XML/include")

ADD_LIBRARY(PocoUtil STATIC ${UTIL_SOURCES})
target_link_libraries(PocoUtil PocoFoundation PocoXML)
