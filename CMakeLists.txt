cmake_minimum_required(VERSION 3.26)
project(chess-analysis C)

set(CMAKE_C_STANDARD 11)

include_directories(
        ${PROJECT_SOURCE_DIR}/src
)

file(GLOB all_SRCS
        "${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/src/*.c"
        )

add_executable(chess-analysis ${all_SRCS})

IF (NOT WIN32)
  target_link_libraries(chess-analysis m)
ENDIF()
