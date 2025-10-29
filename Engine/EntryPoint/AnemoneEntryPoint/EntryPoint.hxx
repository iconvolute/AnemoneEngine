#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneBase/Module.hxx"

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

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneInterop/Windows/Headers.hxx"

extern "C"
{
    ANEMONE_DLLEXPORT extern const UINT D3D12SDKVersion = 614;

#if defined(_M_ARM64)
    ANEMONE_DLLEXPORT extern const char* D3D12SDKPath = ".\\D3D12\\arm64\\";
#elif defined(_M_X64)
    ANEMONE_DLLEXPORT extern const char* D3D12SDKPath = ".\\D3D12\\x64\\";
#endif
}

#endif

#if !defined(ANEMONE_APPLICATION)
#error "This header must be included from the application"
#endif

int AnemoneMain(int argc, char** argv);

#if ANEMONE_PLATFORM_WINDOWS && defined(ANEMONE_APPLICATION_UI)

#include "AnemoneInterop/Windows/Headers.hxx"

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
int WINAPI WinMain(
    [[maybe_unused]] _In_ HINSTANCE hInstance,
    [[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
    [[maybe_unused]] _In_ LPSTR lpCmdLine,
    [[maybe_unused]] _In_ int nShowCmd)
{
    int argc = __argc;
    char** argv = __argv;

    Anemone::Module_Runtime::Initialize();

    int const result = AnemoneMain(argc, argv);

    Anemone::Module_Runtime::Finalize();

    return result;
}

#else

int main(int argc, char** argv)
{
    Anemone::Module_Runtime::Initialize();

    int const result = AnemoneMain(argc, argv);

    Anemone::Module_Runtime::Finalize();

    return result;
}

#endif
