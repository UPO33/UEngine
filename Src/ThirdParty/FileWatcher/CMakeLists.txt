project(FileeWatcher)

file(GLOB SOURCES_FileWatcher "*.cpp")
file(GLOB HEADERS_FileWatcher "*.h")

add_library(FileWatcher STATIC ${SOURCES_FileWatcher} ${HEADERS_FileWatcher})
