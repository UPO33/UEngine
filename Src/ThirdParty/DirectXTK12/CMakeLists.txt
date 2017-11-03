project(DirectXTK12)

file(GLOB SOURCES_DirectXTK12 "*.cpp")
file(GLOB HEADERS_DirectXTK12 "*.h")

#excluding from list
list(REMOVE_ITEM HEADERS_DirectXTK12 "${CMAKE_CURRENT_SOURCE_DIR}/XboxDDSTextureLoader.h")
list(REMOVE_ITEM SOURCES_DirectXTK12 "${CMAKE_CURRENT_SOURCE_DIR}/XboxDDSTextureLoader.cpp")

#Message("${CMAKE_CURRENT_SOURCE_DIR}/XboxDDSTextureLoader.h")
#Message(${HEADERS_DirectXTK12})

add_library(DirectXTK12 STATIC ${SOURCES_DirectXTK12} ${HEADERS_DirectXTK12})
