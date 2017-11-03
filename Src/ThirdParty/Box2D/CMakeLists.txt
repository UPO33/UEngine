project(Box2D)

file(GLOB_RECURSE BOX2D_SOURCES "*.cpp")
file(GLOB_RECURSE BOX2D_HEADERS "*.h")
add_library(Box2D STATIC ${BOX2D_HEADERS} ${BOX2D_SOURCES})
target_include_directories(Box2D PUBLIC "../")