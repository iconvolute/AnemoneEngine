#ifndef ANEMONE_PLATFORM_DETECT_H
#define ANEMONE_PLATFORM_DETECT_H

#include <AnemoneGeneratedConfigurationProperties.hxx>

//--------------------------------------------------------------------------------------------------
// Detect Compiler
//

#if defined(__clang__)
#define ANEMONE_COMPILER_CLANG true
#elif defined(_MSC_VER)
#define ANEMONE_COMPILER_MSVC true
#elif defined(__GNUC__)
#define ANEMONE_COMPILER_GCC true
#else
#error "Unknown compiler"
#endif

#ifndef ANEMONE_COMPILER_CLANG
#define ANEMONE_COMPILER_CLANG false
#endif

#ifndef ANEMONE_COMPILER_CLANGCL
#define ANEMONE_COMPILER_CLANGCL false
#endif

#ifndef ANEMONE_COMPILER_MSVC
#define ANEMONE_COMPILER_MSVC false
#endif

#ifndef ANEMONE_COMPILER_GCC
#define ANEMONE_COMPILER_GCC false
#endif

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Detect Architecture
//

#ifndef ANEMONE_ARCHITECTURE_X64
#define ANEMONE_ARCHITECTURE_X64 false
#endif

#ifndef ANEMONE_ARCHITECTURE_ARM64
#define ANEMONE_ARCHITECTURE_ARM64 false
#endif

#ifndef ANEMONE_ARCHITECTURE_RISCV64
#define ANEMONE_ARCHITECTURE_RISCV64 false
#endif

#ifndef ANEMONE_ARCHITECTURE_RISCV128
#define ANEMONE_ARCHITECTURE_RISCV128 false
#endif

#ifndef ANEMONE_CACHELINE_SIZE
#define ANEMONE_CACHELINE_SIZE 64
#endif

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Detect CPU features
//

#if defined(__AVX__)
#define ANEMONE_FEATURE_AVX true
#endif

#if defined(__AVX2__)
#define ANEMONE_FEATURE_AVX2 true
#define ANEMONE_FEATURE_AVX true
#endif

#if defined(__FMA__) || defined(__AVX2__)
#define ANEMONE_FEATURE_FMA3 true
#endif

#if defined(__F16C__)
#define ANEMONE_FEATURE_F16C true
#endif

#if defined(_M_ARM64) || defined(__ARM_NEON)
#define ANEMONE_FEATURE_NEON true
#endif

#ifndef ANEMONE_FEATURE_AVX
#define ANEMONE_FEATURE_AVX false
#endif

#ifndef ANEMONE_FEATURE_AVX2
#define ANEMONE_FEATURE_AVX2 false
#endif

#ifndef ANEMONE_FEATURE_SVML
#define ANEMONE_FEATURE_SVML false
#endif

#ifndef ANEMONE_FEATURE_FMA3
#define ANEMONE_FEATURE_FMA3 false
#endif

#ifndef ANEMONE_FEATURE_F16C
#define ANEMONE_FEATURE_F16C false
#endif

#ifndef ANEMONE_FEATURE_NEON
#define ANEMONE_FEATURE_NEON false
#endif

#ifndef ANEMONE_FEATURE_ASIMD
#define ANEMONE_FEATURE_ASIMD false
#endif

#ifndef ANEMONE_FEATURE_AESNI
#define ANEMONE_FEATURE_AESNI false
#endif

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Detect Platform
//

#ifndef ANEMONE_PLATFORM_ANDROID
#define ANEMONE_PLATFORM_ANDROID false
#endif

#ifndef ANEMONE_PLATFORM_LINUX
#define ANEMONE_PLATFORM_LINUX false
#endif

#ifndef ANEMONE_PLATFORM_WINDOWS
#define ANEMONE_PLATFORM_WINDOWS false
#endif

#ifndef ANEMONE_PLATFORM_MACOS
#define ANEMONE_PLATFORM_MACOS false
#endif

#ifndef ANEMONE_PLATFORM_IOS
#define ANEMONE_PLATFORM_IOS false
#endif

#ifndef ANEMONE_PLATFORM_EMSCRIPTEN
#define ANEMONE_PLATFORM_EMSCRIPTEN false
#endif

#ifndef ANEMONE_PLATFORM_GAMING_DESKTOP
#define ANEMONE_PLATFORM_GAMING_DESKTOP false
#endif

#ifndef ANEMONE_PLATFORM_GAMING_XBOX_SCARLETT
#define ANEMONE_PLATFORM_GAMING_XBOX_SCARLETT false
#endif

#ifndef ANEMONE_PLATFORM_PS5
#define ANEMONE_PLATFORM_PS5 false
#endif

//
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Platform Kind
//

#if ANEMONE_PLATFORM_WINDOWS || ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_MACOS
#define ANEMONE_PLATFORM_KIND_DESKTOP true
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_IOS
#define ANEMONE_PLATFORM_KIND_MOBILE true
#elif ANEMONE_PLATFORM_EMSCRIPTEN
#define ANEMONE_PLATFORM_KIND_WEB true
#elif ANEMONE_PLATFORM_GAMING_DESKTOP || ANEMONE_PLATFORM_GAMING_XBOX_SCARLETT || ANEMONE_PLATFORM_PS5
#define ANEMONE_PLATFORM_KIND_CONSOLE true
#else
#error "Unknown platform kind"
#endif

#ifndef ANEMONE_PLATFORM_KIND_DESKTOP
#define ANEMONE_PLATFORM_KIND_DESKTOP false
#endif

#ifndef ANEMONE_PLATFORM_KIND_MOBILE
#define ANEMONE_PLATFORM_KIND_MOBILE false
#endif

#ifndef ANEMONE_PLATFORM_KIND_WEB
#define ANEMONE_PLATFORM_KIND_WEB false
#endif

#ifndef ANEMONE_PLATFORM_KIND_CONSOLE
#define ANEMONE_PLATFORM_KIND_CONSOLE false
#endif

//
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// Endianess
//

#define ANEMONE_ENDIAN_LITTLE 1
#define ANEMONE_ENDIAN_BIG 0

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Detect SEH exceptions support
//

#if ANEMONE_COMPILER_MSVC
#define ANEMONE_FEATURE_SEH true
#endif

#ifndef ANEMONE_FEATURE_SEH
#define ANEMONE_FEATURE_SEH false
#endif

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Import/Export symbols for shared libraries
//

#if defined(_MSC_VER)
#define ANEMONE_DLLEXPORT __declspec(dllexport)
#define ANEMONE_DLLIMPORT __declspec(dllimport)
#define ANEMONE_DLLHIDDEN
#else
#define ANEMONE_DLLEXPORT __attribute__((visibility("default")))
#define ANEMONE_DLLIMPORT __attribute__((visibility("default")))
#define ANEMONE_DLLHIDDEN __attribute__((visibility("hidden")))
#endif

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// __vectorcall calling convention
//

#if ANEMONE_COMPILER_MSVC || ANEMONE_COMPILER_CLANGCL || (ANEMONE_COMPILER_CLANG && ANEMONE_PLATFORM_WINDOWS)
#define ANEMONE_FEATURE_VECTORCALL true
#define anemone_vectorcall __vectorcall
#endif

#ifndef ANEMONE_FEATURE_VECTORCALL
#define ANEMONE_FEATURE_VECTORCALL false
#endif

#ifndef anemone_vectorcall
#define anemone_vectorcall
#endif

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// __forceinline support
//

#if ANEMONE_COMPILER_MSVC || ANEMONE_COMPILER_CLANGCL
#define anemone_forceinline __forceinline
#elif ANEMONE_COMPILER_CLANG || ANEMONE_COMPILER_GCC
#define anemone_forceinline __inline __attribute__((__always_inline__))
#else
#define anemone_forceinline inline
#endif

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// noinline support
//

#if ANEMONE_COMPILER_MSVC
#define anemone_noinline __declspec(noinline)
#else
#define anemone_noinline __attribute__((__noinline__))
#endif

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// debugbreak support
//

#if ANEMONE_COMPILER_MSVC
#define anemone_debugbreak __debugbreak
#elif ANEMONE_COMPILER_GCC
#define anemone_debugbreak __builtin_trap
#else
#define anemone_debugbreak __builtin_debugtrap
#endif

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Warning suppression macros
//

#if ANEMONE_COMPILER_MSVC

#define ANEMONE_EXTERNAL_HEADERS_BEGIN \
    _Pragma("warning(push, 0)")

#define ANEMONE_EXTERNAL_HEADERS_END \
    _Pragma("warning(pop)")

#elif ANEMONE_COMPILER_CLANG

#define ANEMONE_EXTERNAL_HEADERS_BEGIN \
    _Pragma("clang diagnostic push"); \
    _Pragma("clang diagnostic ignored \"-Weverything\"")

#define ANEMONE_EXTERNAL_HEADERS_END \
    _Pragma("clang diagnostic pop")

#elif ANEMONE_COMPILER_GCC

#define ANEMONE_EXTERNAL_HEADERS_BEGIN \
    _Pragma("GCC diagnostic push"); \
    _Pragma("GCC diagnostic ignored \"-Wall\""); \
    _Pragma("GCC diagnostic ignored \"-Wextra\"")

#define ANEMONE_EXTERNAL_HEADERS_END \
    _Pragma("GCC diagnostic pop")

#else

#error "Unknown compiler"

#endif

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Detect Sanitizers
//

#ifndef ANEMONE_BUILD_ASAN
#define ANEMONE_BUILD_ASAN false
#endif

#ifndef ANEMONE_BUILD_UBSAN
#define ANEMONE_BUILD_UBSAN false
#endif

#ifndef ANEMONE_BUILD_TSAN
#define ANEMONE_BUILD_TSAN false
#endif

#if ANEMONE_BUILD_ASAN || ANEMONE_BUILD_UBSAN || ANEMONE_BUILD_TSAN
#define ANEMONE_FEATURE_SANITIZERS true
#endif

#ifndef ANEMONE_FEATURE_SANITIZERS
#define ANEMONE_FEATURE_SANITIZERS false
#endif

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Address sanitizer macros
//

#if ANEMONE_BUILD_ASAN
// TODO: Determine if it should be used in single header instead.

#if ANEMONE_COMPILER_MSVC

#define ANEMONE_ASAN_DISABLE(reason) \
    __declspec(no_sanitize_address, noinline)

#else

#define ANEMONE_ASAN_DISABLE(reason) \
    __attribute__((__no_sanitize_address__(reason))

#endif

#endif

#if !defined(ANEMONE_ASAN_DISABLE)
#define ANEMONE_ASAN_DISABLE(...)
#endif

#include <sanitizer/asan_interface.h>

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Configurations
//

#ifndef ANEMONE_CONFIG_DEBUG
#define ANEMONE_CONFIG_DEBUG false
#endif

#ifndef ANEMONE_CONFIG_RELEASE
#define ANEMONE_CONFIG_RELEASE false
#endif

#ifndef ANEMONE_CONFIG_RELWITHDEBINFO
#define ANEMONE_CONFIG_RELWITHDEBINFO false
#endif

#ifndef ANEMONE_CONFIG_MINSIZEREL
#define ANEMONE_CONFIG_MINSIZEREL false
#endif

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Build kinds
//

#ifndef ANEMONE_BUILD_DISABLE_SIMD
#define ANEMONE_BUILD_DISABLE_SIMD false
#endif

#ifndef ANEMONE_BUILD_CODE_ANALYZE
#define ANEMONE_BUILD_CODE_ANALYZE false
#endif

#ifndef ANEMONE_BUILD_DEVELOPER
#define ANEMONE_BUILD_DEVELOPER false
#endif

#ifndef ANEMONE_BUILD_SHIPPING
#define ANEMONE_BUILD_SHIPPING false
#endif

#ifndef ANEMONE_BUILD_TESTING
#define ANEMONE_BUILD_TESTING false
#endif

#ifndef ANEMONE_BUILD_PROFILING
#define ANEMONE_BUILD_PROFILING false
#endif

#ifndef ANEMONE_BUILD_DEVELOPER
#define ANEMONE_BUILD_DEVELOPER false
#endif


#ifndef ANEMONE_BUILD_MONOLITHIC
#define ANEMONE_BUILD_MONOLITHIC false
#endif

//
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Build Features
//

// Stack Trace
#if !ANEMONE_BUILD_SHIPPING || !ANEMONE_FEATURE_SANITIZERS
#if ANEMONE_PLATFORM_ANDROID
#define ANEMONE_FEATURE_STACKTRACE false
#define ANEMONE_FEATURE_STACKTRACE_SYMBOLS false
#else
#define ANEMONE_FEATURE_STACKTRACE true
#define ANEMONE_FEATURE_STACKTRACE_SYMBOLS true
#endif

#endif

#ifndef ANEMONE_FEATURE_STACKTRACE
#define ANEMONE_FEATURE_STACKTRACE false
#endif

#ifndef ANEMONE_FEATURE_STACKTRACE_SYMBOLS
#define ANEMONE_FEATURE_STACKTRACE_SYMBOLS false
#endif

// Event logging
#if !ANEMONE_BUILD_SHIPPING
#define ANEMONE_FEATURE_EVENTLOG true
#endif

#ifndef ANEMONE_FEATURE_EVENTLOG
#define ANEMONE_FEATURE_EVENTLOG false
#endif

// Assertions
#if ANEMONE_BUILD_SHIPPING
#define ANEMONE_FEATURE_ASSERTIONS false
#else
#define ANEMONE_FEATURE_ASSERTIONS true
#endif

#ifndef ANEMONE_FEATURE_ASSERTIONS
#define ANEMONE_FEATURE_ASSERTIONS false
#endif

// Just-in-time debugger support
#if !ANEMONE_BUILD_SHIPPING
#define ANEMONE_FEATURE_JITDEBUGGER true
#endif

//
//--------------------------------------------------------------------------------------------------

#endif // ANEMONE_PLATFORM_DETECT_H
