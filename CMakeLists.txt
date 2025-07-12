cmake_minimum_required(VERSION 3.27)

# Project setup
project(idn-16 C)
set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find required packages for assembler
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

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


# Generate parser and lexer files
set(ASM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/tools/assembler)
set(ASM_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(PARSER_OUTPUT ${ASM_BUILD_DIR}/parser.tab.c)
set(PARSER_HEADER ${ASM_BUILD_DIR}/parser.tab.h)
set(LEXER_OUTPUT ${ASM_BUILD_DIR}/lex.yy.c)

# Bison for parser
add_custom_command(
	OUTPUT ${PARSER_OUTPUT} ${PARSER_HEADER}
	COMMAND ${BISON_EXECUTABLE} -Wcex -d -o ${PARSER_OUTPUT} ${ASM_DIR}/parser.y
	DEPENDS ${ASM_DIR}/parser.y
	COMMENT "Generating parser with Bison"
)

# Flex for lexer  
add_custom_command(
	OUTPUT ${LEXER_OUTPUT}
	COMMAND ${FLEX_EXECUTABLE} -o ${LEXER_OUTPUT} ${ASM_DIR}/lexer.l
	DEPENDS ${ASM_DIR}/lexer.l ${PARSER_HEADER}
	COMMENT "Generating lexer with Flex"
)

# Core source files
set(CORE_SOURCES
	src/core/cpu.c
	src/core/memory.c
	src/core/instructions.c
	src/core/syscalls.c
	src/core/io/display.c
	src/core/io/keyboard.c
	src/core/io/audio.c
	src/tools/disassembler/dasm.c
)

# SFD source files
set(SFD_SOURCES
	lib/sfd/sfd.c
)

# === MAIN EMULATOR ===
set(EMULATOR_SOURCES
	${LEXER_OUTPUT}
	${PARSER_OUTPUT}
	src/tools/assembler/asm_main.c
	src/tools/assembler/codegen.c
	src/tools/assembler/symbol_table.c
	src/main.c
	${CORE_SOURCES}
	${SFD_SOURCES}
)

add_executable(${PROJECT_NAME} ${EMULATOR_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
	include
	lib/clay
	lib/sfd
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${FLEX_LIBRARIES}
	SDL3::SDL3
	SDL3_ttf::SDL3_ttf
	SDL3_image::SDL3_image
	m
)

# === DISASSEMBLER ===
set(DISASSEMBLER_SOURCES
	src/tools/disassembler/disassembler.c
	src/tools/disassembler/dasm.c
)

add_executable(idn16-dasm ${DISASSEMBLER_SOURCES})
target_include_directories(idn16-dasm PRIVATE include)

# === TESTS ===
enable_testing()

# Unity test framework
set(UNITY_SOURCES tests/unity/unity.c)

# Memory tests
add_executable(test_memory 
	tests/core/test_memory.c
	${UNITY_SOURCES}
	src/core/memory.c
	src/core/instructions.c
	src/core/cpu.c
	src/core/syscalls.c
	src/tools/disassembler/dasm.c
)
target_include_directories(test_memory PRIVATE include tests/unity)
target_link_libraries(test_memory PRIVATE SDL3::SDL3)
add_test(NAME memory_test COMMAND test_memory)

# CPU tests
add_executable(test_cpu
	tests/core/test_cpu.c  
	${UNITY_SOURCES}
	src/core/cpu.c
	src/core/memory.c
	src/core/instructions.c
	src/core/syscalls.c
	src/tools/disassembler/dasm.c
)
target_include_directories(test_cpu PRIVATE include tests/unity)
target_link_libraries(test_cpu PRIVATE SDL3::SDL3)
add_test(NAME cpu_test COMMAND test_cpu)

# Symbol table tests
add_executable(test_symbol_table
	tests/tools/test_symbol_table.c
	${UNITY_SOURCES}
	src/tools/assembler/symbol_table.c
)
target_include_directories(test_symbol_table PRIVATE include tests/unity)
add_test(NAME symbol_table_test COMMAND test_symbol_table)

# Disassembler tests  
add_executable(test_disassembler
	tests/tools/test_disassembler.c
	${UNITY_SOURCES}
	src/tools/disassembler/dasm.c
)
target_include_directories(test_disassembler PRIVATE include tests/unity)
add_test(NAME disassembler_test COMMAND test_disassembler)

# Codegen tests
add_executable(test_codegen
	tests/tools/test_codegen.c
	${UNITY_SOURCES}
	src/tools/assembler/codegen.c
	src/tools/assembler/symbol_table.c
)
target_include_directories(test_codegen PRIVATE include tests/unity)
add_test(NAME codegen_test COMMAND test_codegen)

# Syscalls tests
add_executable(test_syscalls
	tests/core/test_syscalls.c
	${UNITY_SOURCES}
	src/core/cpu.c
	src/core/memory.c
	src/core/instructions.c
	src/core/syscalls.c
	src/tools/disassembler/dasm.c
)
target_include_directories(test_syscalls PRIVATE include tests/unity)
target_link_libraries(test_syscalls PRIVATE SDL3::SDL3)
add_test(NAME syscalls_test COMMAND test_syscalls)

# Audio tests
add_executable(test_audio
	tests/core/test_audio.c
	${UNITY_SOURCES}
	src/core/cpu.c
	src/core/memory.c
	src/core/instructions.c
	src/core/syscalls.c
	src/tools/disassembler/dasm.c
)
target_include_directories(test_audio PRIVATE include tests/unity)
target_link_libraries(test_audio PRIVATE SDL3::SDL3)
add_test(NAME audio_test COMMAND test_audio)

# Error handling tests
add_executable(test_error_handling
	tests/core/test_error_handling.c
	${UNITY_SOURCES}
	src/core/cpu.c
	src/core/memory.c
	src/core/instructions.c
	src/core/syscalls.c
	src/tools/disassembler/dasm.c
)
target_include_directories(test_error_handling PRIVATE include tests/unity)
target_link_libraries(test_error_handling PRIVATE SDL3::SDL3)
add_test(NAME error_handling_test COMMAND test_error_handling)

# === RESOURCE COPYING ===
add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/resources
		${CMAKE_CURRENT_BINARY_DIR}/resources
	COMMENT "Copying resources to build directory"
)

# === CUSTOM TARGETS ===
# Build all tools
add_custom_target(tools DEPENDS idn16-dasm)

# Build everything
add_custom_target(all_targets DEPENDS ${PROJECT_NAME} tools)

# Run all tests
add_custom_target(run_tests COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

# Clean generated files
add_custom_target(clean_generated
	COMMAND ${CMAKE_COMMAND} -E remove -f ${LEXER_OUTPUT} ${PARSER_OUTPUT} ${PARSER_HEADER}
	COMMENT "Cleaning generated parser/lexer files"
)