cmake_minimum_required(VERSION 3.10)
project(chipsim LANGUAGES C VERSION 1.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(${PROJECT_NAME}
src/main.c
src/chip.c
src/logic_gate.c
)
