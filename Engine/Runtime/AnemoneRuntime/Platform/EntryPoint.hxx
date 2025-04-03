#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

// GPU power management markers.
extern "C"
{
    // NVIDIA: http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
    // ReSharper disable once CppNonInlineVariableDefinitionInHeaderFile
    ANEMONE_DLLEXPORT unsigned NvOptimusEnablement = 0x00000001;

    // AMD: http://devgurus.amd.com/thread/169965
    // ReSharper disable once CppNonInlineVariableDefinitionInHeaderFile
    ANEMONE_DLLEXPORT int AmdPowerXpressRequestHighPerformance = 1;
}

#if !defined(ANEMONE_APPLICATION)
#error "This header must be included from the application"
#endif

#include "AnemoneRuntime/Runtime.hxx"

int AnemoneMain(int argc, char** argv);

#if ANEMONE_PLATFORM_WINDOWS && defined(ANEMONE_APPLICATION_UI)

#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
int WINAPI WinMain(
    [[maybe_unused]] _In_ HINSTANCE hInstance,
    [[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
    [[maybe_unused]] _In_ LPSTR lpCmdLine,
    [[maybe_unused]] _In_ int nShowCmd)
{
    int argc = __argc;
    char** argv = __argv;

    Anemone::Runtime::Initialize(argc, argv);

    int const result = AnemoneMain(argc, argv);

    Anemone::Runtime::Finalize();

    return result;
}

#else

int main(int argc, char** argv)
{
    Anemone::Runtime::Initialize(argc, argv);

    int const result = AnemoneMain(argc, argv);

    Anemone::Runtime::Finalize();

    return result;
}

#endif
