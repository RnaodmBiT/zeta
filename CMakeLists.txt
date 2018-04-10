cmake_minimum_required(VERSION 3.9)

project(zeta)

function(set_working_directory PROJECT_NAME USERFILE_WORKING_DIRECTORY)
    set(USER_FILE "${PROJECT_NAME}.vcxproj.user")
    set(OUTPUT_PATH ${USER_FILE})
    configure_file(${CMAKE_SOURCE_DIR}/vcxproj.user.template ${USER_FILE} @ONLY)
endfunction(set_working_directory)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Require OpenGL
find_package(OpenGL REQUIRED)

# Add glm maths
include_directories(glm)

# Add SDL2
add_subdirectory(SDL2-2.0.4)
target_include_directories(SDL2-static PUBLIC SDL2-2.0.4/include)

# Add GLEW
add_subdirectory(glew)

# make a NANOVG project
file(GLOB NANO_SRC nanovg/src/*)
add_library(nanovg ${NANO_SRC})
target_include_directories(nanovg PUBLIC nanovg/src)


# define the list of external libraries
set(LIBRARIES
    ${OPENGL_LIBRARIES}
    SDL2-static
    SDL2main
    glew
    nanovg)

# Add the common library
file(GLOB_RECURSE COMMON_SRC common/*)
add_library(common ${COMMON_SRC})
target_link_libraries(common ${LIBRARIES})
target_include_directories(common PUBLIC common)


# Add the editor executable
file(GLOB_RECURSE EDITOR_SRC editor/*)
add_executable(editor ${EDITOR_SRC})
target_link_libraries(editor common)
set_working_directory(editor ${CMAKE_CURRENT_SOURCE_DIR}/bin)
