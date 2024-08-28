# Build Types
option(ANEMONE_BUILD_DISABLE_SIMD       "Disable SIMD optimizations" OFF)
option(ANEMONE_BUILD_ASAN               "Enable Address Sanitizer" OFF)
option(ANEMONE_BUILD_UBSAN              "Enable Undefined Sanitizer" OFF)
option(ANEMONE_BUILD_TSAN               "Enable Thread Sanitizer" OFF)

option(ANEMONE_BUILD_MONOLITHIC         "Monolithic Build" OFF)
option(ANEMONE_BUILD_DEVELOPER          "Build with developer features" OFF)
option(ANEMONE_BUILD_SHIPPING           "Build for shipping" OFF)
option(ANEMONE_BUILD_PROFILING          "Build with profiling" OFF)
