#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Debugger.hxx"

#include <string_view>
#include <fmt/format.h>

namespace Anemone
{
    struct WindowsDebugger final
    {
        static void HandleCrash(PEXCEPTION_POINTERS pExceptionPointers);

        static void PrintRaw(const char* message);

        static void PrintFormatted(std::string_view format, fmt::format_args args);

        template <typename... Args>
        static void Print(std::string_view format, Args const&... args)
        {
            PrintFormatted(format, fmt::make_format_args(args...));
        }
    };
}
