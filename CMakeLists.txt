cmake_minimum_required(VERSION 3.16)
project(chipsim LANGUAGES C VERSION 1.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src)
add_executable(chipsim
src/main.c
src/app.c
src/core/camera.c
src/core/event.c
src/core/input.c
src/core/window.c
src/simulation/ctx.c
src/simulation/connection.c
src/simulation/circuit.c
src/simulation/simulate.c
src/unit.c
src/editor/editor.c
src/editor/editor_ui.c
src/editor/render.c
src/textures.c
src/ui.c
)

target_precompile_headers(chipsim PRIVATE
    <string.h>
    <stdlib.h>
    <math.h>
    <stdio.h>
    <stdint.h>
)

target_link_libraries(chipsim PRIVATE SDL3::SDL3)
target_link_libraries(chipsim PRIVATE SDL3_ttf::SDL3_ttf)

install(DIRECTORY "asset/" DESTINATION share/chipsimap)

if(WIN32)
    add_custom_command(TARGET chipsim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:chipsim>
            $<TARGET_FILE_DIR:chipsim>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLL dependencies..."
    )
endif()