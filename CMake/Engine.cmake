if (ANEMONE_BUILD_MONOLITHIC)
    set(ANEMONE_MODULE_KIND STATIC)
else()
    set(ANEMONE_MODULE_KIND SHARED)
endif()

function(_anemone_target_generate_metadata target_name)
    file(GLOB_RECURSE metadata_inputs
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.hxx
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
    )

    # Define custom command to generate file
    add_custom_command(
        OUTPUT
            "${CMAKE_CURRENT_BINARY_DIR}/Generated/Metadata.cxx"
        COMMAND
            ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/Generated"
        COMMAND
            AnemoneMetadataGenerator
            "${target_name}"
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${CMAKE_CURRENT_BINARY_DIR}/Generated/Metadata.cxx"
        DEPENDS
            "${metadata_inputs}"
            AnemoneMetadataGenerator
        COMMENT "Generating metadata ${CMAKE_CURRENT_BINARY_DIR}/Generated/Metadata.cxx"
        VERBATIM
    )

    target_sources(${target_name}
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}/Generated/Metadata.cxx"
    )

endfunction()

function(_anemone_collect_dependencies out_list target)
    if(NOT TARGET "${target}")
        return()
    endif()

    # Avoid duplicates
    list(FIND ${out_list} "${target}" already)
    if(already GREATER -1)
        return()
    endif()

    # Query direct deps
    get_target_property(direct_deps "${target}" LINK_LIBRARIES)

    if(direct_deps)
        foreach(dep IN LISTS direct_deps)
            _anemone_collect_dependencies(${out_list} "${dep}")
        endforeach()
    endif()

    # After its dependencies → append the target itself
    list(APPEND ${out_list} "${target}")
    set(${out_list} "${${out_list}}" PARENT_SCOPE)
endfunction()


function(anemone_generate_code)
    # Generate executable initializers/finalizers
    get_property(all_executables GLOBAL PROPERTY ANEMONE_ALL_EXECUTABLES)
    foreach (executable IN LISTS all_executables)
        _anemone_executable_bootstrap(${executable})
    endforeach()
endfunction()

function(_anemone_executable_bootstrap target_name)
    get_target_property(target_binary_dir ${target_name} BINARY_DIR)

    _anemone_collect_dependencies(target_dependencies ${target_name})

    set(source_initialize)
    set(source_finalize)

    foreach (dependency IN LISTS target_dependencies)
        if (TARGET ${dependency})
            get_target_property(module_initializer ${dependency} ANEMONE_MODULE_INITIALIZER)
            get_target_property(module_finalizer   ${dependency} ANEMONE_MODULE_FINALIZER)
            get_target_property(module_export_macro   ${dependency} ANEMONE_EXPORT_MACRO)

            if (NOT module_export_macro)
                set(module_export_macro "/* local */")
            endif()

            if (module_initializer)
                string(APPEND source_initialize "    ${module_export_macro} extern void ${module_initializer}();\n")
                string(APPEND source_initialize "    ${module_initializer}();\n")
            endif()
            if (module_finalizer)
                string(PREPEND source_finalize "    ${module_finalizer}();\n")
                string(PREPEND source_finalize "    ${module_export_macro} extern void ${module_finalizer}();\n")
            endif()
        endif()
    endforeach()

    set(source_generated "${target_binary_dir}/Generated/ModuleInitializer.cxx")

    file(WRITE ${source_generated}
"// This file is auto-generated. Do not edit directly.
#include \"AnemoneRuntime.Interop/Headers.hxx\"

extern void Anemone_InitializeExecutable()
{
${source_initialize}
}

extern void Anemone_FinalizeExecutable()
{
${source_finalize}
}
")

    target_sources(${target_name}
        PRIVATE
            ${source_generated}
    )

endfunction()


function(anemone_add_target)

    set(options
        STATIC              # Create a static module
        SHARED              # Create a shared module
        MODULE              # Create a static/shared module
        HEADER              # Create a headeronly module
        EXECUTABLE          # Create a console application
        APPLICATION         # Create an UI executable
        TEST                # Create a test executable
        SDK
    )

    set(oneValueArgs
        NAME                # Name of the target
        ALIAS               # An alias used to reference the target
        EXPORT              # Export definition for that target
    )

    set(multiValueArgs
        BUILD_DEPENDENCIES      # List of build dependencies
        RUNTIME_DEPENDENCIES    # List of runtime dependencies
    )
    
    cmake_parse_arguments(anemone_add_target "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT anemone_add_target_NAME)
        message(FATAL_ERROR "anemone_add_target: NAME argument is required")
    endif()

    set(anemone_add_target_INSTALL FALSE)

    if (anemone_add_target_STATIC)
        add_library(${anemone_add_target_NAME} STATIC)
    elseif(anemone_add_target_SHARED)
        add_library(${anemone_add_target_NAME} SHARED)
        set(anemone_add_target_INSTALL TRUE)
    elseif(anemone_add_target_MODULE)
        add_library(${anemone_add_target_NAME} ${ANEMONE_MODULE_KIND})
        if (${ANEMONE_MODULE_KIND} MATCHES "SHARED")
            set(anemone_add_target_INSTALL TRUE)
        endif()
    elseif(anemone_add_target_HEADER)
        add_library(${anemone_add_target_NAME} INTERFACE)
    elseif(anemone_add_target_EXECUTABLE)
        add_executable(${anemone_add_target_NAME})
        target_compile_definitions(${anemone_add_target_NAME}
            PRIVATE
                "ANEMONE_APPLICATION"
        )
        set(anemone_add_target_INSTALL TRUE)
    elseif(anemone_add_target_APPLICATION)
        add_executable(${anemone_add_target_NAME})
        set(anemone_add_target_INSTALL TRUE)
        target_compile_definitions(${target_name}
            PRIVATE
                "ANEMONE_APPLICATION"
                "ANEMONE_APPLICATION_UI"
        )
        if (WIN32)
            set_target_properties(${anemone_add_target_NAME}
                PROPERTIES
                    WIN32_EXECUTABLE TRUE
            )
        endif()
    elseif(anemone_add_target_TEST)
        add_executable(${anemone_add_target_NAME})
        set(anemone_add_target_INSTALL TRUE)
        target_compile_definitions(${anemone_add_target_NAME}
            PRIVATE
                "ANEMONE_APPLICATION"
                "ANEMONE_APPLICATION_CONSOLE"
                "ANEMONE_APPLICATION_TEST"
        )
        add_test(
            NAME ${anemone_add_target_NAME}
            COMMAND ${anemone_add_target_NAME}
        )

        target_link_libraries(${anemone_add_target_NAME} PRIVATE SdkCatch2)
    endif()

    if (anemone_add_target_SDK)
        if (anemone_add_target_HEADER)
            target_sources(${anemone_add_target_NAME} INTERFACE FILE_SET HEADERS BASE_DIRS "include")
        else()
            target_sources(${anemone_add_target_NAME} PUBLIC FILE_SET HEADERS BASE_DIRS "include")

            if (WIN32 OR MSVC)
                target_compile_definitions(${anemone_add_target_NAME} PRIVATE "_CRT_SECURE_NO_WARNINGS")
            endif()
        endif()
    else()    
        # Add include directory
        target_sources(${anemone_add_target_NAME} PUBLIC FILE_SET HEADERS BASE_DIRS .)
    endif()

    if (anemone_add_target_INSTALL)
        install(TARGETS ${anemone_add_target_NAME} RUNTIME DESTINATION bin)

        if(WIN32)
            install(FILES $<TARGET_PDB_FILE:${anemone_add_target_NAME}> DESTINATION bin OPTIONAL)
        endif()
    endif()

    if (anemone_add_target_ALIAS)
        add_library(${anemone_add_target_ALIAS} ALIAS ${anemone_add_target_NAME})
    endif()

    if (anemone_add_target_EXPORT)
        set_target_properties(${anemone_add_target_NAME} PROPERTIES
            ANEMONE_EXPORT_MACRO ${anemone_add_target_EXPORT}
        )

        if (NOT ANEMONE_BUILD_MONOLITHIC)
            target_compile_definitions(${anemone_add_target_NAME}
                PRIVATE
                    "${anemone_add_target_EXPORT}=ANEMONE_DLLEXPORT"
                INTERFACE
                    "${anemone_add_target_EXPORT}=ANEMONE_DLLIMPORT"
            )
        else()
            target_compile_definitions(${anemone_add_target_NAME}
                PUBLIC
                    "${anemone_add_target_EXPORT}="
            )
        endif()
    endif()

    # Collect all executables that need to be processed for metadata generation
    if (anemone_add_target_TEST OR anemone_add_target_APPLICATION OR anemone_add_target_EXECUTABLE)
        set_property(GLOBAL APPEND PROPERTY ANEMONE_ALL_EXECUTABLES ${anemone_add_target_NAME})
    endif()


    # Make C++ symbol safe name of the module.
    string(REGEX REPLACE "[.+-]" "_" anemone_add_target_NAME_SYMBOL "${anemone_add_target_NAME}")
    set_target_properties(${anemone_add_target_NAME} PROPERTIES
        ANEMONE_TARGET_NAME_SYMBOL "${anemone_add_target_NAME_SYMBOL}"
    )

    # Anemone Modules provide initialize/finalize functions and generated metadata.
    if (NOT (anemone_add_target_SDK OR anemone_add_target_HEADER))
        set_target_properties(${anemone_add_target_NAME} PROPERTIES
            ANEMONE_MODULE_INITIALIZER "${anemone_add_target_NAME_SYMBOL}_ModuleInitialize"
            ANEMONE_MODULE_FINALIZER "${anemone_add_target_NAME_SYMBOL}_ModuleFinalize"
        )
    endif()

    set_property(GLOBAL APPEND PROPERTY ANEMONE_ALL_TARGETS ${anemone_add_target_NAME})

    if (anemone_add_target_BUILD_DEPENDENCIES)
        list(REMOVE_DUPLICATES anemone_add_target_BUILD_DEPENDENCIES)
        target_link_libraries(${anemone_add_target_NAME} ${anemone_add_target_BUILD_DEPENDENCIES})
    endif()

    if (NOT anemone_add_target_SDK AND NOT anemone_add_target_HEADER)
        if (NOT ANEMONE_BUILD_CODE_ANALYZE)
            set_target_properties(
                ${anemone_add_target_NAME}
                PROPERTIES
                    COMPILE_WARNING_AS_ERROR TRUE
            )
            if(MSVC)
                target_compile_options(
                    ${anemone_add_target_NAME}
                    PRIVATE
                        /Wall       # enable all warnings
                        #-WL         # show warnings as messages
                        /wd4710
                        /wd4711
                        ################
                        /wd4514
                        /wd4371 # layout of class may have changed from a previous version of the compiler due to better packing of member
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
                    ${anemone_add_target_NAME}
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
                        -Wno-unused-const-variable
                )
            endif()
        endif()
    endif()

    # ASan support
    if (ANEMONE_BUILD_ASAN)
        if (MSVC)
            target_compile_options(${anemone_add_target_NAME} PUBLIC -fsanitize=address)
        else()
            target_link_options(${anemone_add_target_NAME} PUBLIC -fsanitize=address)
            target_compile_options(${anemone_add_target_NAME} PUBLIC -fsanitize=address)
        endif()
    endif()

    # UBSan support
    if (ANEMONE_BUILD_UBSAN)
        if (MSVC)
            message(FATAL_ERROR "UBSan is not yet available on MSVC")
        else()
            target_link_options(${anemone_add_target_NAME} PUBLIC -fsanitize=undefined)
            target_compile_options(${anemone_add_target_NAME} PUBLIC -fsanitize=undefined)
        endif()
    endif()

    # TSan support
    if (ANEMONE_BUILD_TSAN)
        if (MSVC)
            message(FATAL_ERROR "TSan is not yet available on MSVC")
        else()
            target_link_options(${anemone_add_target_NAME} PUBLIC -fsanitize=thread)
            target_compile_options(${anemone_add_target_NAME} PUBLIC -fsanitize=thread)
        endif()
    endif()

    # Code analyzer
    if (ANEMONE_BUILD_CODE_ANALYZE)
        if (MSVC)
            target_compile_options(${anemone_add_target_NAME} PUBLIC /analyze)
        endif()
    endif()

endfunction()
#
#function(anemone_get_base_target_name target_name output)
#    if (NOT TRAGET ${target_name})
#        message(FATAL_ERROR "anemone_get_base_target_name: target ${target_name} does not exist")
#    endif()
#
#    while(target_name)
#
#    endwhile()
#endfunction()
#

# Detect GIT properties
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE ANEMONE_ENGINE_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE ANEMONE_ENGINE_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Configure whole project
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/AnemoneGeneratedConfigurationProperties.hxx" @ONLY
)
include_directories("${CMAKE_CURRENT_BINARY_DIR}/include")
