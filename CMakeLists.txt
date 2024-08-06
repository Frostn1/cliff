cmake_minimum_required(VERSION 3.16)
project(cliff LANGUAGES C)
set(CMAKE_C_STANDARD 11)

file(GLOB LEVEL_SOURCE src/**/*.c)
file(GLOB SOURCE src/*.c)

add_executable(cliff
    ${LEVEL_SOURCE}
    ${SOURCE}
)


target_include_directories(cliff PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/lexer
    ${CMAKE_SOURCE_DIR}/src/parser

)



set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
