if (ANEMONE_BUILD_MONOLITHIC)
    set(ANEMONE_MODULE_KIND STATIC)
else()
    set(ANEMONE_MODULE_KIND SHARED)
endif()

function(_anemone_target_add_options target_name)
    target_compile_definitions(${target_name}
        # Platform
        PRIVATE
            $<$<BOOL:${ANEMONE_PLATFORM_WINDOWS}>:-DANEMONE_PLATFORM_WINDOWS=1>
            $<$<BOOL:${ANEMONE_PLATFORM_LINUX}>:-DANEMONE_PLATFORM_LINUX=1>
            $<$<BOOL:${ANEMONE_PLATFORM_ANDROID}>:-DANEMONE_PLATFORM_ANDROID=1>

        # Architecture
        PRIVATE
            $<$<BOOL:${ANEMONE_ARCHITECTURE_ARM64}>:-DANEMONE_ARCHITECTURE_ARM64=1>
            $<$<BOOL:${ANEMONE_ARCHITECTURE_X64}>:-DANEMONE_ARCHITECTURE_X64=1>

        # Sanitizers
        PRIVATE
            $<$<BOOL:${ANEMONE_BUILD_ASAN}>:-DANEMONE_BUILD_ASAN=1>
            $<$<BOOL:${ANEMONE_BUILD_UBSAN}>:-DANEMONE_BUILD_UBSAN=1>
            $<$<BOOL:${ANEMONE_BUILD_TSAN}>:-DANEMONE_BUILD_TSAN=1>

        # Build
        PRIVATE
            $<$<BOOL:${ANEMONE_BUILD_DISABLE_SIMD}>:-DANEMONE_BUILD_DISABLE_SIMD=1>
            $<$<BOOL:${ANEMONE_BUILD_MONOLITHIC}>:-DANEMONE_BUILD_MONOLITHIC=1>
            $<$<BOOL:${ANEMONE_BUILD_DEVELOPER}>:-DANEMONE_BUILD_DEVELOPER=1>
            $<$<BOOL:${ANEMONE_BUILD_SHIPPING}>:-DANEMONE_BUILD_SHIPPING=1>
            $<$<BOOL:${ANEMONE_BUILD_PROFILING}>:-DANEMONE_BUILD_PROFILING=1>
            $<$<BOOL:${ANEMONE_BUILD_MONOLITIC}>:-DANEMONE_BUILD_MONOLITIC=1>

        # Features
        PRIVATE
            $<$<BOOL:${ANEMONE_FEATURE_SVML}>:-DANEMONE_FEATURE_SVML=1>

        # Configuration
        PRIVATE
            $<$<CONFIG:DEBUG>:-DANEMONE_CONFIG_DEBUG=1>
            $<$<CONFIG:RELEASE>:-DANEMONE_CONFIG_RELEASE=1>
    )

    # ASan support
    if (ANEMONE_BUILD_ASAN)
        if (MSVC)
            target_compile_options(${target_name} PUBLIC -fsanitize=address)
        else()
            target_compile_options(${target_name} PUBLIC -fsanitize=address)
        endif()
    endif()

    # UBSan support
    if (ANEMONE_BUILD_UBSAN)
        if (MSVC)
            message(FATAL_ERROR "UBSan is not yet available on MSVC")
        else()
            target_compile_options(${target_name} PUBLIC -fsanitize=undefined)
        endif()
    endif()

    # TSan support
    if (ANEMONE_BUILD_TSAN)
        if (MSVC)
            message(FATAL_ERROR "TSan is not yet available on MSVC")
        else()
            target_compile_options(${target_name} PUBLIC -fsanitize=thread)
        endif()
    endif()

    # Code analyzer
    if (ANEMONE_BUILD_CODE_ANALYZE)
        if (MSVC)
            target_compile_options(${target_name} PUBLIC /analyze)
        endif()
    endif()

    # Configure compiler
    if (MSVC)
        target_link_options(${target_name}
            PRIVATE
                # /CETCOMPAT          # Enable Control Flow Guard compatibility
                /debug:fastlink                             # Generate debug information
                $<$<NOT:$<CONFIG:DEBUG>>:/OPT:REF>          # Remove unreferenced functions and data.
                $<$<NOT:$<CONFIG:DEBUG>>:/OPT:ICF>          # Identical COMDAT folding.
                $<$<CONFIG:DEBUG>:/OPT:NOREF>               # No unreferenced data elimination.
                $<$<CONFIG:DEBUG>:/OPT:NOICF>               # No Identical COMDAT folding.
        )

        target_compile_options(${target_name}
            PRIVATE
                #/GR-                                       # Disable RTTI
                $<$<NOT:$<CONFIG:RELEASE>>:/sdl>            # Enable additional security features; useful for catching some bugs
                $<$<NOT:$<CONFIG:RELEASE>>:/guard:cf>       # Enable Control Flow Guard
                $<$<CONFIG:RELEASE>:/sdl- /GS->             # Disable additional security features
                $<$<CONFIG:RELEASE>:/Gy>                    # Enable COMDAT folding
                $<$<CONFIG:RELEASE>:/Gw>                    # Optimize Global Data
                /ZH:SHA_256                                 # Use SHA-256 hash for PDB
                /fp:fast                                    # Fast floating-point optimizations
                /permissive-                                # Enable strict conformance mode
                /volatile:iso
                /Zc:__cplusplus                             # Enable __cplusplus macro
                /Zc:inline                                  # Enable inline conformance
                /Zc:preprocessor                            # Enable preprocessor conformance
                /Zc:lambda                                  # Enable lambda conformance
                /Zc:checkGwOdr                              # Check global variable definitions
                /Zc:referenceBinding
                /Zc:rvalueCast
                /Zc:strictStrings
                /Zc:ternary
                /Zc:wchar_t
                /Zc:forScope                                # conformant for-scope
                /Zc:enumTypes
                /Zc:externConstexpr
                /Zc:throwingNew
                /external:anglebrackets                     # External includes passed in `<>`
                /external:W0                                # Disable warnings for external includes
                /utf-8                                      # Set source and execution character set to UTF-8
                /MP                                         # multicore compilation
        )

        target_compile_definitions(${target_name}
            PUBLIC
                UNICODE
                _UNICODE
        )

    else()
        target_compile_options(${target_name}
            PRIVATE
                #-g
                -ggdb
                #-ffast-math                                 # Enable fast math optimizations
        )
    endif()

    # Features
    if (MSVC)
        if(ANEMONE_ARCHITECTURE_X64)
            target_compile_options(
                ${target_name}
                PRIVATE
                    /arch:AVX
                    /favor:AMD64
            )
            target_compile_definitions(
                ${target_name}
                PRIVATE
                    __AVX__=1
            )
        endif()

        if(ANEMONE_ARCHITECTURE_ARM64)
            target_compile_options(
                ${target_name}
                PRIVATE
                    /arch:armv8.1
                    /Zc:arm64-aliased-neon-types-
            )

            target_compile_definitions(
                ${target_name}
                PRIVATE
                    _ARM64_DISTINCT_NEON_TYPES
            )
        endif()
    else()
        if (ANEMONE_ARCHITECTURE_X64)
            target_compile_options(
                ${target_name}
                PRIVATE
                    -mavx
            )
            target_compile_definitions(
                ${target_name}
                PRIVATE
                    __AVX__=1
            )
        endif()

        if(ANEMONE_ARCHITECTURE_ARM64)
            if (NOT ANDROID)
                target_compile_options(
                    ${target_name}
                    PRIVATE
                        -march=native
                        -mtune=native
                        -mcpu=native
                )
                target_compile_definitions(
                    ${target_name}
                    PRIVATE
                        __ARM_NEON=1
                )
            endif()
        endif()
    endif()

endfunction()

function(_anemone_target_enable_warnings target_name)
    if (NOT ANEMONE_BUILD_CODE_ANALYZE)
        set_target_properties(
            ${target_name}
            PROPERTIES
                COMPILE_WARNING_AS_ERROR TRUE
        )
        if(MSVC)
            target_compile_options(
                ${target_name}
                PRIVATE
                    /diagnostics:caret
                    /Wall       # enable all warnings
                    #-WL         # show warnings as messages
                    /wd4710
                    /wd4711
                    ################
                    /wd4514
                    /wd4582
                    /wd4820
                    /wd4365
                    /wd5264
                    /wd5039
                    /wd5026
                    /wd4625
                    /wd4626
                    /wd5027
                    /wd4623
                    /wd5045
                    /wd4583
                    /wd4587
                    /wd4588
                    /wd4574
                    /wd4686
                    /wd4868
                    /wd4251
                    /wd4061 # Not all labels are EXPLICITLY handled in switch; w4062 (not all labels are handled and default is missing) is enabled
                    #-wd4668 # regression in vs2022.17.8
                    /wd4883
                    /wd5246 # the initialization of a subobject should be wrapped in braces; valid in C++
                    /wd5267 # definition of implicit assignment operator for is deprecated because it has a user-provided destructor
                    /wd4324 # structure was padded due to alignment specifier
                    /wd4746 # volatile access of '<expression>' is subject to /volatile:[iso|ms] setting; consider using __iso_volatile_load/store intrinsic functions.
                    /wd5030 # attribute [[xxx]] is not recognized
            )
        else()
            target_compile_options(
                ${target_name}
                PRIVATE
                    -Wall
                    -Wextra
                    #-Weverything
                    #-Wno-trigraphs
                    #-Wno-c++98-compat
                    #-Wno-c++20-compat
                    #-Wno-c++98-compat-pedantic
                    #-Wno-pre-c++14-compat
                    #-Wno-pre-c++17-compat
                    #-Wno-pre-c++20-compat
                    #-Wno-padded
                    #-Wno-missing-variable-declarations
                    #-Wno-float-equal
                    #-Wno-unsafe-buffer-usage
                    #-Wno-documentation-unknown-command
                    #-Wno-switch-default
                    #-Wno-double-promotion
                    #-Wno-undefined-func-template
                    -Wno-attributes
                    -Wno-missing-field-initializers
            )
        endif()
    endif()

endfunction()

function(_anemone_target_add_includes target_name)
    target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

function(_anemone_target_install target_name)
    install(TARGETS ${target_name} DESTINATION bin)

    if(WIN32)
        install(FILES $<TARGET_PDB_FILE:${target_name}> DESTINATION bin OPTIONAL)
    endif()
endfunction()


function(anemone_add_module target_name)

    # Parse arguments
    set(options
        STATIC
    )

    set(namedOptions
        ALIAS
        EXPORT
    )

    set(variadicOptions)

    cmake_parse_arguments(
        parsed
            "${options}"
            "${namedOptions}"
            "${variadicOptions}"
            ${ARGN}
    )

    set(target_kind ${ANEMONE_MODULE_KIND})

    if (parsed_STATIC)
        set(target_kind STATIC)
    endif()


    # Create target
    add_library(${target_name} ${target_kind})
    _anemone_target_add_options(${target_name})
    _anemone_target_enable_warnings(${target_name})
    _anemone_target_add_includes(${target_name})

    if (NOT target_kind MATCHES STATIC)
        _anemone_target_install(${target_name})
    endif()

    if (parsed_ALIAS)
        add_library(${parsed_ALIAS} ALIAS ${target_name})
    endif()

    if (parsed_EXPORT)
        if (NOT ANEMONE_BUILD_MONOLITHIC)
            target_compile_definitions(${target_name}
                PRIVATE
                    "${parsed_EXPORT}=ANEMONE_DLLEXPORT"
                INTERFACE
                    "${parsed_EXPORT}=ANEMONE_DLLIMPORT"
            )
        else()
            target_compile_definitions(${target_name}
                PUBLIC
                    "${parsed_EXPORT}="
            )
        endif()
    endif()


endfunction()

function(_anemone_add_executable target_name)
    add_executable(${target_name})
    _anemone_target_add_options(${target_name})
    _anemone_target_enable_warnings(${target_name})
    _anemone_target_install(${target_name})
    _anemone_target_add_includes(${target_name})

    target_compile_definitions(${target_name}
        PRIVATE
            "ANEMONE_APPLICATION"
    )

endfunction()


function(anemone_add_ui_executable target_name)
    _anemone_add_executable(${target_name})

    if (WIN32)
    set_target_properties(${target_name}
        PROPERTIES
            WIN32_EXECUTABLE TRUE
    )
    endif()

    target_compile_definitions(${target_name}
        PRIVATE
            "ANEMONE_APPLICATION_UI"
    )

endfunction()

function(anemone_add_console_executable target_name)
    _anemone_add_executable(${target_name})

    target_compile_definitions(${target_name}
        PRIVATE
            "ANEMONE_APPLICATION_CONSOLE"
    )

endfunction()

function(anemone_add_test target_name)
    _anemone_add_executable(${target_name})

    target_compile_definitions(${target_name}
        PRIVATE
            "ANEMONE_APPLICATION_CONSOLE"
            "ANEMONE_APPLICATION_TEST"
    )

    add_test(
        NAME ${target_name}
        COMMAND ${target_name}
    )

    target_link_libraries(${target_name} PRIVATE SdkCatch2)
endfunction()

function(anemone_add_sdk target_name target_kind)
    add_library(${target_name} ${target_kind})

    if(${target_kind} MATCHES "INTERFACE")
        target_include_directories(${target_name} INTERFACE "include")
    else()
        target_include_directories(${target_name} PUBLIC "include")

        if (WIN32 OR MSVC)
            target_compile_definitions(${target_name} PRIVATE "_CRT_SECURE_NO_WARNINGS")
        endif()
    endif()

    if(${target_kind} MATCHES "SHARED")
        _anemone_target_install(${target_name})
    endif()

    _anemone_target_add_options(${target_name})
endfunction()

# Detect GIT properties
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE ANEMONE_ENGINE_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_QUIET
    ERROR_QUIET
)

execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE ANEMONE_ENGINE_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_QUIET
    ERROR_QUIET
)

# Configure whole project
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/AnemoneGeneratedConfigurationProperties.hxx" @ONLY
)
include_directories("${CMAKE_CURRENT_BINARY_DIR}/include")
