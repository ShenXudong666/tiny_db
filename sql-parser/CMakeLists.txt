cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_VERSION VERSION_LESS "3.3")
    # Don't ignore visibility related properties for non-SHARED targets
    cmake_policy(SET CMP0063 NEW)
endif()

if (NOT CMAKE_VERSION VERSION_LESS "3.13")
    # CMP0077: option() honors normal variables
    # https://cmake.org/cmake/help/latest/policy/CMP0077.html
    cmake_policy(SET CMP0077 NEW)
endif()

project (sqlparser)

# The version number.
set (SQLPARSER_VERSION_MAJOR 1)
set (SQLPARSER_VERSION_MINOR 0)

#Build type
#set(CMAKE_BUILD_TYPE Debug)

#Core include directories 

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(SQLPARSER_LIBRARIES ${LIB_OUTPUT_NAME} CACHE INTERNAL "")
set(SQLPARSER_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "")


#Core Sources  
file(GLOB ROOT_SRC "src/*.cpp")
file(GLOB PARSER_SRC "src/parser/*.cpp")
file(GLOB SQL_SRC "src/sql/*.cpp")
file(GLOB UTIL_SRC "src/util/*.cpp")

#append to main SRC
list(APPEND SOURCES ${ROOT_SRC}) 
list(APPEND SOURCES ${PARSER_SRC})  
list(APPEND SOURCES ${SQL_SRC})  
list(APPEND SOURCES ${UTIL_SRC})  

#Compile library
LINK_DIRECTORIES(${LINKER_DIRS})
add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#Install 
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)
install (TARGETS ${LIB_OUTPUT_NAME} ARCHIVE DESTINATION lib  LIBRARY DESTINATION lib  RUNTIME DESTINATION lib COMPONENT library)
