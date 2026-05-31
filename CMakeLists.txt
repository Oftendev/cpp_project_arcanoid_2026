cmake_minimum_required(VERSION 3.16)

project(MyArcanoidProject VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(BUILD_SHARED_LIBS FALSE)
add_definitions(-DPATH_TO_DATA="${CMAKE_CURRENT_SOURCE_DIR}/")

FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.2
)

FetchContent_MakeAvailable(sfml)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)


configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/arial.ttf"
    "${CMAKE_CURRENT_BINARY_DIR}/arial.ttf"
    COPYONLY
)