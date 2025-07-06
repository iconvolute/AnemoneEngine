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


#[[

Project structure described below will be most useful once we transition to C++ modules.

/CMake
/ThirdParty
/Samples
/Engine
    /Runtime/"AnemoneRuntime"       -- module `Anemone.Runtime`
        /Application                    -- implements platform-specific application event queue and window handling
        /Crypto                         -- basic cryptographic primitives
        /Diagnostics                    -- diagnostics and logging
        /Input                          -- input system (based on Application module, forwards input events?)
        /Memory                         -- memory management
        /Mesh                           -- mesh primitives (known vertices, basic manipulation APIs)
        /Modules                        -- dynamic modules system
        /Network                        -- network primitives (sockets, client/server)
        /Numerics                       -- math primitives, SIMD implementations
        /Profiler                       -- runtime profiler
        /Reflection                     -- reflection system (C++2x based) + RTTI at runtime (custom)
        /Resources                      -- resource and asset management
        /Serialization                  -- serialization implementation (uses RTTI and code generation)
        /Storage                        -- file system primitives
        /Streaming                      -- async streaming system; works with resource system
        /System                         -- system primitives (current process info, system info, etc)
        /Tasks                          -- task system
        /Threading                      -- multithreading primitives
    /Platform/"AnemonePlatform"     -- meta-module; will disapear once we switch to C++ modules
        /Windows
            /Headers.hxx
            /Functions.hxx
            /Types.hxx
        /Linux
        /Android
        /Posix
        /GamingDesktop
    /Tests/                         -- runtime test applications
/Modules
    # Engine Modules
    /Animation
    /AI
    /Audio
    /Physics
    /Video
    /Rendering
    /Graphics
        /D3D12
        /Vulkan
    /ShaderSystem
    /ShaderSystemEditor
    /UI
    /VM
    /VersionControl/GIT
    /VersionControl/Perforce    
/Editor                            -- editor application / framework?
    /Framework/"AnemoneEditorFramework" -- module `Anemone.Editor.Framework`
        /Windowing                          -- manages editor multi-window UI
        /Application                        -- implementation of editor application
        /Undo                               -- undo/redo system
        /PropertyInspector                  -- property inspector UI
        /VersionControl                     -- version control
    /App/"AnemoneEditor"            -- module `Anemone.Editor`
        Main.cxx                            -- entry point
/Tests - generic ones? or should it be like in for each module / runtime / application?
/Tools
    /CrashReporter                  -- crash repoorter application

            Runtime/                            -- runtime library
                Hash/
                Geometry/
                Rendering/
                RHI/                            -- null, vulkan, d3d12
                Graphics/
                Camera/
            Modules/                            -- additional modules, developed separately
                AI                              -- BT, utility AI, etc
                AIEditor/
                AssetDatabase/
                Animation/
                Audio/
                AssetPipelineEditor/
                AudioEditor/
                Cloth/
                CrashReporting/
                Image/
                UI/
                Input/
                JsonSerialize/
                JsonSerializeEditor/
                Localization/
                Multiplayer/
                PackageManager/
                PerformanceReporting/
                ParticleSystem/
                Physics/
                PhysicsEditor/
                ScreenCapture/
                ShaderSystem/
                ShaderEditor/
                Terrain/
                TerrainPhysics/
                TerrainEditor/
                InspectorEditor/
                TextRendering/
                Tilemap/
                BuildPipeline/
                BuildReportingEditor/
                Media/
                Video/
            Tools/
            Tests/
-- library parts
    - native
        - cxx/hxx - sources headers, in normal parts
    - bindings
        - C# bindings
    - managed
        - C# implementation
    - generated?
]]
