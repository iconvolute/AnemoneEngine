#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"

namespace Anemone
{
    struct WindowsEnvironment final
    {
        // Returns the value indicating if the application is a console application.
        static RUNTIME_API bool IsConsoleApplication();

        // Returns the value indicating if console output is redirected.
        static RUNTIME_API bool IsConsoleRedirecting();

        static RUNTIME_API bool IsOnline();
    };
}
