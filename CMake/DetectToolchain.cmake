include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
include(CheckSymbolExists)

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
endif()



# Detect SVML feature
check_cxx_source_compiles("
#include <immintrin.h>

int main() {
    __m128 v = _mm_set_ps1(0.6f);
    __m128 f = _mm_svml_floor_ps(v);
    return static_cast<int>(_mm_cvtss_f32(f));
}" HAVE_SVML)

option(ANEMONE_FEATURE_SVML "Enable SVML" ${HAVE_SVML})
message(STATUS "Feature SVML: ${ANEMONE_FEATURE_SVML}")


function(detect_architecture symbol arch)
    if (NOT DEFINED ARCHITECTURE)
        set(CMAKE_REQUIRED_QUIET 1)
        check_symbol_exists("${symbol}" "" ARCHITECTURE_${arch})
        unset(CMAKE_REQUIRED_QUIET)

        if (ARCHITECTURE_${arch})
            set(ARCHITECTURE "${arch}" PARENT_SCOPE)
            set(ANEMONE_ARCHITECTURE_${arch} 1 PARENT_SCOPE)
        endif()
    endif()
endfunction()

if (MSVC)
    check_symbol_exists("_M_ARM64" "" ANEMONE_ARCHITECTURE_ARM64)
    check_symbol_exists("_M_AMD64" "" ANEMONE_ARCHITECTURE_X64)
else()
    check_symbol_exists("__aarch64__" "" ANEMONE_ARCHITECTURE_ARM64)
    check_symbol_exists("__x86_64__" "" ANEMONE_ARCHITECTURE_X64)
endif()
