cmake_minimum_required(VERSION 3.27)

# Project setup
project(idn-16 C)
set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# Download SDL3
FetchContent_Declare(
	SDL
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG release-3.2.4
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
message(STATUS "Using SDL via FetchContent")
FetchContent_MakeAvailable(SDL)
set_property(DIRECTORY "${sdl_SOURCE_DIR}" PROPERTY EXCLUDE_FROM_ALL TRUE)

# Download SDL_ttf
FetchContent_Declare(
	SDL_ttf
	GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
	GIT_TAG release-3.2.2
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
message(STATUS "Using SDL_ttf via FetchContent")
FetchContent_MakeAvailable(SDL_ttf)
set_property(DIRECTORY "${sdl_ttf_SOURCE_DIR}" PROPERTY EXCLUDE_FROM_ALL TRUE)

# Download SDL_image
FetchContent_Declare(
	SDL_image
	GIT_REPOSITORY "https://github.com/libsdl-org/SDL_image.git"
	GIT_TAG release-3.2.0
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
message(STATUS "Using SDL_image via FetchContent")
FetchContent_MakeAvailable(SDL_image)
set_property(DIRECTORY "${SDL_image_SOURCE_DIR}" PROPERTY EXCLUDE_FROM_ALL TRUE)



# Core source files
set(CORE_SOURCES
	src/core/cpu.c
	src/core/memory.c
	src/core/instructions.c
	src/core/runner.c
	src/core/io/display.c
	src/core/io/keyboard.c
	src/tools/disassembler/dasm.c
)

# Clay UI source files
set(CLAY_SOURCES
	lib/clay/SDL3/clay_renderer_SDL3.c
)

# Clay UI source files
set(SFD_SOURCES
	lib/sfd/sfd.c
)

set(SOURCES 
	src/main.c
	${CORE_SOURCES}
	${SFD_SOURCES}
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
	include
	lib/clay
	lib/sfd
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	SDL3::SDL3
	SDL3_ttf::SDL3_ttf
	SDL3_image::SDL3_image
)

add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/resources
		${CMAKE_CURRENT_BINARY_DIR}/resources
)