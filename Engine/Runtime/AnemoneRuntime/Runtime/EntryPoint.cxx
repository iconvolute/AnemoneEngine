#include "AnemoneRuntime/Interop/Headers.hxx"

namespace Anemone::Internal
{
    extern int GCommandLineArgC;
    extern char** GCommandLineArgV;
    extern bool GIsConsoleApplication;

    extern void InitializeDiagnostics();
    extern void FinalizeDiagnostics();

    extern void InitializeEnvironment();
    extern void FinalizeEnvironment();

    extern void InitializeClipboard();
    extern void FinalizeClipboard();

    //extern void InitializeThreading();
    //extern void FinalizeThreading();

    extern void InitializeTaskScheduler();
    extern void FinalizeTaskScheduler();

    extern void InitializeFileSystem();
    extern void FinalizeFileSystem();

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

    Anemone::Internal::InitializeDiagnostics();
    Anemone::Internal::InitializeEnvironment();
    Anemone::Internal::InitializeFileSystem();
    Anemone::Internal::InitializeClipboard();
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
    Anemone::Internal::FinalizeClipboard();
    Anemone::Internal::FinalizeFileSystem();
    Anemone::Internal::FinalizeEnvironment();
    Anemone::Internal::FinalizeDiagnostics();
}
