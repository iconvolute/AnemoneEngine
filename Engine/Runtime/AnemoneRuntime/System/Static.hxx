#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::System
{
    extern UninitializedObject<Environment> GEnvironment;
    extern UninitializedObject<ProcessorProperties> GProcessorProperties;

    struct GenericSystemStatic
    {
        static void Initialize();
        static void Finalize();
    };
}

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/System/Windows/Static.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/System/Posix/Static.hxx"
#else
#error "Unsupported platform"
#endif
