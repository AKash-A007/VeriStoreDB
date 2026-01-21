cmake_minimum_required(VERSION 3.14)
project(VeriStoreDB VERSION 1.0 LANGUAGES CXX)

# ---------------------------------------
# Compiler Settings
# ---------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)   # good for static libs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------
# Dependencies - CLI11
# ---------------------------------------
include(FetchContent)

FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG v2.3.2
)

option(USING_SYSTEM_CLI11 "Use system-installed CLI11" OFF)

if(NOT USING_SYSTEM_CLI11)
  FetchContent_MakeAvailable(cli11)
else()
  find_package(CLI11 QUIET)
endif()

# ---------------------------------------
# VeriStoreDB Core Library
# ---------------------------------------
add_library(VeriStoreDB_core
    src/db/database.cpp

    # Add subsystem source files here as they are created:
    # src/btree/btree.cpp
    # src/wal/wal.cpp
    # src/gitstore/gitstore.cpp
    # src/utils/file_utils.cpp
)

# Public include (for users of the library)
target_include_directories(VeriStoreDB_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

# Warnings for core library
if(MSVC)
    target_compile_options(VeriStoreDB_core PRIVATE /W4)
else()
    target_compile_options(VeriStoreDB_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---------------------------------------
# VSDB Command-Line Executable
# ---------------------------------------
add_executable(vsdb
    src/main.cpp
    src/cli/cli_parser.cpp
)

target_link_libraries(vsdb
    PRIVATE
        VeriStoreDB_core
        CLI11::CLI11
)

target_include_directories(vsdb
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# ---------------------------------------
# Installation (optional)
# ---------------------------------------
install(TARGETS vsdb RUNTIME DESTINATION bin)
install(TARGETS VeriStoreDB_core ARCHIVE DESTINATION lib)

# ---------------------------------------
# Testing (optional)
# ---------------------------------------
enable_testing()
