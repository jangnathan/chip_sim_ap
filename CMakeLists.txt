cmake_minimum_required(VERSION 3.10)
project(chipsim LANGUAGES C VERSION 1.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_executable(chipsim
src/main.c
src/app.c
src/render.c
src/chip.c
src/logic_gate.c
src/runtime.c
src/unit.c
src/ui.c
)

target_link_libraries(chipsim PRIVATE SDL3::SDL3)
target_link_libraries(chipsim PRIVATE SDL3_ttf::SDL3_ttf)

FILE(COPY "asset/" DESTINATION "${CMAKE_BINARY_DIR}/asset/")
