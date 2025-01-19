#pragma once
#include "AnemoneRuntime/Platform/Platform.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::Platform
{
    RUNTIME_API void ReportApplicationStop(std::string_view reason);
    RUNTIME_API bool IsProcessEmulated();
    RUNTIME_API void HandleCrash(EXCEPTION_POINTERS* pExceptionPointers);
};

namespace Anemone::Platform::Internal
{
    struct WindowsPlatformStatics final
    {
        HINSTANCE InstanceHandle{};

        HICON ApplicationIcon{};

        HCURSOR ArrowCursor{};
        HCURSOR IBeamCursor{};
        HCURSOR WaitCursor{};
        HCURSOR CrossCursor{};
        HCURSOR SizeNWSECursor{};
        HCURSOR SizeNESWCursor{};
        HCURSOR SizeWECursor{};
        HCURSOR SizeNSCursor{};
        HCURSOR SizeAllCursor{};

        ATOM MainWindowClass{};
    };

    RUNTIME_API extern UninitializedObject<WindowsPlatformStatics> GWindowsPlatformStatics;
}
