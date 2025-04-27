#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

#include <string_view>
#include <fmt/format.h>

namespace Anemone::Internal
{
    class WindowsDebuggerStatics final
    {
    private:
        LPTOP_LEVEL_EXCEPTION_FILTER m_PreviousExceptionFilter{};

    public:
        WindowsDebuggerStatics();
        ~WindowsDebuggerStatics();

        static void HandleCrash(PEXCEPTION_POINTERS pExceptionPointers);
    };

    extern UninitializedObject<WindowsDebuggerStatics> GDebuggerStatics;
}
