cmake_minimum_required(VERSION 3.10)
project(chipsim LANGUAGES C VERSION 1.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

include_directories(src)
add_executable(chipsim
src/main.c
src/app.c
src/chip.c
src/simulate.c
src/event.c
src/unit.c
src/camera.c
src/input.c
src/editor/editor.c
src/editor/render.c
src/textures.c
src/ui.c
)

target_link_libraries(chipsim PRIVATE SDL3::SDL3)
target_link_libraries(chipsim PRIVATE SDL3_ttf::SDL3_ttf)

install(DIRECTORY "asset/" DESTINATION share/chipsimap)
