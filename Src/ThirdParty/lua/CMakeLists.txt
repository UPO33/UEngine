project(Lua)

file(GLOB SOURCES_Lua "*.c")
file(GLOB HEADERS_Lua "*.h")

add_library(Lua STATIC ${SOURCES_Lua} ${HEADERS_Lua})
