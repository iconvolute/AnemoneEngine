#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Diagnostic/Debug.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Diagnostic
{
    struct GenericDiagnosticStatic
    {
        static void Initialize();
        static void Finalize();
    };
}

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Diagnostic/Windows/Static.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Diagnostic/Posix/Static.hxx"
#else
#error "Unsupported platform"
#endif
