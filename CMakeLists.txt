cmake_minimum_required(VERSION 3.27)
project(IDN-16 C)
set(CMAKE_C_STANDARD 99)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
    GIT_TAG "release-2.30.10"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(
    SDL2_ttf
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf.git"
    GIT_TAG "release-2.22.0"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL2_ttf)

FetchContent_Declare(
    SDL2_image
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL_image.git"
    GIT_TAG "release-2.8.4"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL2_image)

add_executable(IDN-16 src/main.c)

target_compile_options(IDN-16 PUBLIC)
target_include_directories(IDN-16 PUBLIC .)

target_link_libraries(IDN-16 PUBLIC
    SDL2::SDL2main
    SDL2::SDL2-static
    SDL2_ttf::SDL2_ttf-static
    SDL2_image::SDL2_image-static
)

if(MSVC)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
else()
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
endif()

add_custom_command(
    TARGET IDN-16 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts
    ${CMAKE_CURRENT_BINARY_DIR}/resources/fonts
)