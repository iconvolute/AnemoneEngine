#include "AnemoneRuntime.Diagnostics/Trace.hxx"
#include "AnemoneRuntime.Diagnostics/Debug.hxx"
#include "AnemoneRuntime.System/Environment.hxx"
#include "AnemoneRuntime.System/ProcessorProperties.hxx"
#include "AnemoneRuntime.Storage/FileSystem.hxx"
#include "AnemoneRuntime.System/Clipboard.hxx"
#include "AnemoneRuntime.Tasks/TaskScheduler.hxx"
#include "AnemoneRuntime.Profiler/Profiler.hxx"

ANEMONE_RUNTIME_BASE_API extern void AnemoneRuntime_Base_ModuleInitialize()
{
    Anemone::Trace::Initialize();
    Anemone::Debug::Initialize();

#if ANEMONE_BUILD_PROFILING
    Anemone::Profiler::Initialize();
#endif

    Anemone::Environment::Initialize();
    Anemone::ProcessorProperties::Initialize();
    Anemone::FileSystem::Initialize();
    Anemone::Clipboard::Initialize();
    Anemone::TaskScheduler::Initialize();
}

ANEMONE_RUNTIME_BASE_API extern void AnemoneRuntime_Base_ModuleFinalize()
{
    Anemone::TaskScheduler::Finalize();
    Anemone::Clipboard::Finalize();
    Anemone::FileSystem::Finalize();
    Anemone::ProcessorProperties::Finalize();
    Anemone::Environment::Finalize();

#if ANEMONE_BUILD_PROFILING
    Anemone::Profiler::Finalize();
#endif

    Anemone::Debug::Finalize();
    Anemone::Trace::Finalize();
}
