cmake_minimum_required(VERSION 3.0.0)
project(blaze VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_library(blaze src/blaze.cpp src/blaze_html.cpp)
target_compile_definitions(blaze PUBLIC BLAZE_VERSION=\"${PROJECT_VERSION}\")

add_executable(blaze-test src/test/main.cpp)
target_link_libraries(blaze-test blaze)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
