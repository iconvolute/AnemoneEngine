#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseDebugger.hxx"

#include <string_view>
#include <fmt/format.h>

namespace Anemone
{
    struct WindowsDebugger final
    {
        static void Break();
        static void Crash();
        static bool IsAttached();
        static void Wait();
        static bool Attach();

        static void ReportApplicationStop(std::string_view reason);
        static bool IsProcessEmulated();
        static void HandleCrash(PEXCEPTION_POINTERS pExceptionPointers);


        static void PrintRaw(const char* message);

        static void PrintFormatted(std::string_view format, fmt::format_args args);

        template <typename... Args>
        static void Print(std::string_view format, Args const&... args)
        {
            PrintFormatted(format, fmt::make_format_args(args...));
        }
    };

    using Debugger = WindowsDebugger;
}
