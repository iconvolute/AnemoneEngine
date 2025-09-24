#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Storage/FileSystem.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/System/Clipboard.hxx"

namespace Anemone::Internal
{
    extern int GCommandLineArgC;
    extern char** GCommandLineArgV;
    extern bool GIsConsoleApplication;

    extern void InitializeEnvironment();
    extern void FinalizeEnvironment();

    //extern void InitializeThreading();
    //extern void FinalizeThreading();

    extern void InitializeTaskScheduler();
    extern void FinalizeTaskScheduler();

    //extern void InitializeApplication();
    //extern void FinalizeApplication();

    extern void InitializeInput();
    extern void FinalizeInput();

    extern void InitializeProfiling();
    extern void FinalizeProfiling();
}

extern "C" RUNTIME_API void AnemoneRuntimeInitialize(int argc, char** argv, bool console)
{
    Anemone::Internal::GCommandLineArgC = argc;
    Anemone::Internal::GCommandLineArgV = argv;
    Anemone::Internal::GIsConsoleApplication = console;

    Anemone::Trace::Initialize();
    Anemone::Debug::Initialize();
    Anemone::Internal::InitializeEnvironment();
    Anemone::FileSystem::Initialize();
    Anemone::Clipboard::Initialize();

    //Anemone::Internal::InitializeThreading();
    Anemone::Internal::InitializeTaskScheduler();
    Anemone::Internal::InitializeProfiling();
    //Anemone::Internal::InitializeApplication();
    Anemone::Internal::InitializeInput();
}

extern "C" RUNTIME_API void AnemoneRuntimeFinalize()
{
    Anemone::Internal::FinalizeInput();
    //Anemone::Internal::FinalizeApplication();
    Anemone::Internal::FinalizeProfiling();
    Anemone::Internal::FinalizeTaskScheduler();
    //Anemone::Internal::FinalizeThreading();

    Anemone::Clipboard::Finalize();
    Anemone::FileSystem::Finalize();

    Anemone::Internal::FinalizeEnvironment();
    Anemone::Debug::Finalize();
    Anemone::Trace::Finalize();
}
