# Validate 64-bit target platform
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
message(FATAL_ERROR "64-bit builds are not supported")
endif()

# Set PDB format for MSVC
if(MSVC)
    if(ANEMONE_BUILD_ASAN)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded)
    else()
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT ProgramDatabase)
    endif()
endif()

# Replace default inlining optimization level
if (MSVC)
string(REPLACE "/Ob2" "/Ob3" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
string(REPLACE "/Ob2" "/Ob3" CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}")
endif()

if (MSVC)
    if (CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL "ARM64")
        set(ANEMONE_ARCHITECTURE_ARM64 ON)
    elseif (CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL "x64")
        set(ANEMONE_ARCHITECTURE_X64 ON)
        set(ANEMONE_FEATURE_SVML ON)
    endif()
endif()

if (CLANG OR GCC)
    if (CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL "aarch64")
        set(ANEMONE_ARCHITECTURE_ARM64 ON)
    elseif (CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL "x86_64")
        set(ANEMONE_ARCHITECTURE_X64 ON)
        set(ANEMONE_FEATURE_SVML ON)
    endif()
endif()

message(STATUS "Feature SVML: ${ANEMONE_FEATURE_SVML}")
