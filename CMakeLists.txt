cmake_minimum_required(VERSION 3.27)

# Shared libraries define export macros
set(CMAKE_SHARED_LIBRARY_ENABLE_EXPORTS TRUE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable whole program optimization
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)

# Enable link time code generation
set(CMAKE_OPTIMIZE_DEPENDENCIES_RELEASE TRUE)
set(CMAKE_OPTIMIZE_DEPENDENCIES_RELWITHDEBINFO TRUE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

cmake_policy(SET CMP0141 NEW)

project(AnemoneEngine VERSION 1.0.0.0)

enable_testing()
enable_language(C CXX)

include("CMake/Options.cmake")
include("CMake/DetectToolchain.cmake")
include("CMake/Engine.cmake")

add_library(AnemoneGlobalOptions INTERFACE)

if (MSVC)
    target_compile_options(AnemoneGlobalOptions INTERFACE /W4 /WX)
endif()

add_subdirectory("Engine")
add_subdirectory("Samples")
add_subdirectory("Tests")
add_subdirectory("ThirdParty")


#[[

Layout: this way it would be the easiest to have transition to C++ modules later on.
 
/CMake
/ThirdParty
/Samples
/Engine
    /Runtime                                            -- module `Anemone.Runtime`;
        -- don't need to introduce any separation layer for just include paths
        ## modules layout ##
        ## VVVVVVVVVVVVVV ##
        /Threading
            /Windows/Thread.cxx
            /Posix/Thread.cxx
            /Thread.hxx
            /Thread.cxx
        ## ^^^^^^^^^^^^^^ ##

        -- 'AnemoneRuntime' is a namespace prefix for headers, 'Anemone.Runtime' is a module name when moved to modules
        ## headers layout ##
        ## VVVVVVVVVVVVVV ##
        ## ^^^^^^^^^^^^^^ ##
        /AnemoneRuntime
            /Threading
                /Windows/Thread.cxx
                /Posix/Thread.cxx
                /Thread.hxx
                /Thread.cxx
    /Platform                                           -- module `Anemone.Platform`; // is this really a module?
    /AI
    /Audio
    /Physics
    /Video
    /Rendering
    /Graphics
/Editor
/Tools


/engine
    /platform
        /Anemone
            /Platform
                /Detect.hxx
                /Windows
                /Linux
                /Android
                /Posix
                /GamingDesktop
    /runtime
        /Anemone/
            /Runtime
                /Threading/Thread.hxx
                /Threading/Thread.cxx
                /Threading/Windows/Thread.cxx
                /Tasks/TaskScheduler.hxx
    /geometry
        /Anemone
			/Geometry
				/Vector.hxx
				/Matrix.hxx
    /ui
    /graphics
    /rendering
    /vm
    /ai
    /audio
    /physics
/editor
/tools
/tests

-- this way include path looks like this:

/engine/runtime/
      #include "AnemoneRuntime/Threading/Thread.hxx"

/engine/runtime/AnemoneRuntime/Threading/Thread.hxx"
/engine/platform/
       #include "AnemonePlatform/Windows/Types.hxx"





    code/
        runtime/
            platform/                                   -- header only library with platform specific definitions
				CMakeLists.txt                          -- defines target AnemonePlatform: include = cwd;
				AnemonePlatform/                        -- incldue directory prefix for headers
					Windows/
						Platform.hxx:                   -- `#include "AnemonePlatform/Windows/Platform.hxx"`
						Platform.cxx
					Linux/
						Platform.hxx
						Platform.cxx
					Android/
						Platform.hxx
						Platform.cxx
					Posix/
						Platform.hxx
						Platform.cxx
            core/                                       -- actual runtime implementation
                CMakeLists.txt                          -- defines target AnemoneRuntime: include = cwd;
                AnemoneRuntime/                         -- incldue directory prefix for headers
                    Threading/
                        Windows/
                            Thread.cxx:                 -- `#include "AnemoneRuntime/Threading/Thread.hxx"`
                    Application/
                        Windows/
                            Application.cxx             -- `#include "AnemoneRuntime/Application/Application.hxx"`
            geometry/
            graphics/
            rendering/
            ui/
            vm/
        editor/
        tools/


    sdk/
    runtime/
        Anemone/
            Runtime/
                FileXxx1.hxx
                FileXxx1.cxx
        Anemone.UI/
        Anemone.AI

/
    cmake/                                      -- cmake setup files
    sdk/                                        -- external dependencies
    source/                                     -- sources
        Anemone/                                -- common prefix for includes
            Platform/                           -- platform and configuration headers
                Linux/
                Windows/
                Android/
                Posix/
            CrashReporter/                      -- crash reporter application
            Editor/                             -- editor application? where to put editor modules then?
                Windowing/
                Application/
                Undo/
                PropertyInspector/
                VersionControl/
            Runtime/                            -- runtime library
                Application/
                Crypto/
                Diagnostics/
                Hash/
                Memory/
                Network/
                Numerics/
                Storage/
                System/
                Tasks/
                Threading/
                Reflection/                     -- rtti
                Serialization/                  -- serialization implementation
                Geometry/
                Rendering/
                RHI/                            -- null, vulkan, d3d12
                Profiler/
                Streaming/
                Input/
                Graphics/
                Logging/
                Math/
                Modules/
                Network/
                Plugins/
                Camera/
                Resources/
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
    dev/                                        -- development source code (tests, tools etc)

-- library parts
    - native
        - cxx/hxx - sources headers, in normal parts
    - bindings
        - C# bindings
    - managed
        - C# implementation
    - generated?
]]
