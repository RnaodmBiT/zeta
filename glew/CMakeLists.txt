
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(glew glew.c)
target_include_directories(glew PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})