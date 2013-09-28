set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB JSON_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	)
set(JSON_SPIRIT_SRCS ${JSON_SOURCES})

#SET(JSON_SPIRIT_SRCS
#    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
#	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"

#${CMAKE_CURRENT_SOURCE_DIR}/json_spirit_reader.cpp ${CMAKE_CURRENT_SOURCE_DIR}/json_spirit_reader.h
#${CMAKE_CURRENT_SOURCE_DIR}/json_spirit_value.cpp ${CMAKE_CURRENT_SOURCE_DIR}/json_spirit_value.h
#${CMAKE_CURRENT_SOURCE_DIR}/json_spirit_writer.cpp ${CMAKE_CURRENT_SOURCE_DIR}/json_spirit_writer.h
#${CMAKE_CURRENT_SOURCE_DIR}/json_spirit.h
#${CMAKE_CURRENT_SOURCE_DIR}/json_spirit_error_position.h
#${CMAKE_CURRENT_SOURCE_DIR}/json_spirit_reader_template.h
#${CMAKE_CURRENT_SOURCE_DIR}/json_spirit_stream_reader.h
#${CMAKE_CURRENT_SOURCE_DIR}/json_spirit_utils.h
#${CMAKE_CURRENT_SOURCE_DIR}/json_spirit_writer_options.h
#${CMAKE_CURRENT_SOURCE_DIR}/json_spirit_writer_template.h
#PARENT_SCOPE)
 
#FIND_PACKAGE(Boost 1.50 REQUIRED)
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

ADD_LIBRARY(json_spirit STATIC ${JSON_SOURCES})

