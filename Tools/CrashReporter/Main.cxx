#include "AnemoneRuntime/Instant.hxx"
#include "AnemoneRuntime/Platform/Main.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"
#include "AnemoneRuntime/Threading/Yielding.hxx"
#include "AnemoneRuntime/Platform/Runtime.hxx"

#include <fmt/format.h>

int main(int argc, char* argv[])
{
    Anemone::Platform::Runtime runtime{argc, argv};
    Anemone::Platform::SetCrashReportHandling(false);

    for (int i = 0; i < argc; ++i)
    {
        fmt::println("argv[{}]: '{}'", i, argv[i]);
    }

    return 0;
}
