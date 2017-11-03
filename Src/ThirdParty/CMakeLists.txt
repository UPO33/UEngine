
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})

project(ThirthParty)

set(OUT_DIR_NAME "Win32")

if(WIN32)
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
		set(OUT_DIR_NAME "Win32")
	else()
		set(OUT_DIR_NAME "Win64")
	endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Lib/${OUT_DIR_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Lib/${OUT_DIR_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Lib/${OUT_DIR_NAME})

add_subdirectory("lua")
#add_subdirectory("zlib")
add_subdirectory("Clipper")
add_subdirectory("ConvertUTF")
#add_subdirectory("assimp")
add_subdirectory("Box2D")
add_subdirectory("FileWatcher")
add_subdirectory("DirectXTK12")