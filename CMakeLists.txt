cmake_minimum_required(VERSION 3.27)

# Shared libraries define export macros
set(CMAKE_SHARED_LIBRARY_ENABLE_EXPORTS TRUE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable whole program optimization
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

# Enable link time code generation
set(CMAKE_OPTIMIZE_DEPENDENCIES_RELEASE TRUE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

cmake_policy(SET CMP0141 NEW)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(AnemoneEngine VERSION 1.0.0.0)

enable_testing()
enable_language(C CXX)

# 1. Find external dependencies.
include("CMake/Modules/FindITT.cmake")

find_package(Vulkan)

# 2. Detect toolchain and set up compiler flags.
include("CMake/DetectToolchain.cmake")

# 3. Configure build options and generate files.
include("CMake/Options.cmake")

# 4. Setup engine commands.
include("CMake/Engine.cmake")


add_subdirectory("Engine")
add_subdirectory("Samples")
add_subdirectory("Tests")
add_subdirectory("ThirdParty")
add_subdirectory("Tools")
