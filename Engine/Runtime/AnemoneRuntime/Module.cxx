#include "AnemoneRuntime/Module.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/System/ProcessorProperties.hxx"
#include "AnemoneRuntime/Storage/FileSystem.hxx"
#include "AnemoneRuntime/System/Clipboard.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"

namespace Anemone
{
    void Module_Runtime::Initialize()
    {
        Anemone::Trace::Initialize();
        Anemone::Debug::Initialize();
        Anemone::Environment::Initialize();
        Anemone::ProcessorProperties::Initialize();
        Anemone::FileSystem::Initialize();
        Anemone::Clipboard::Initialize();

#if ANEMONE_BUILD_PROFILING
        Anemone::Profiler::Initialize();
#endif
    }

    void Module_Runtime::Finalize()
    {
#if ANEMONE_BUILD_PROFILING
        Anemone::Profiler::Finalize();
#endif

        Anemone::Clipboard::Finalize();
        Anemone::FileSystem::Finalize();
        Anemone::ProcessorProperties::Finalize();
        Anemone::Environment::Finalize();
        Anemone::Debug::Finalize();
        Anemone::Trace::Finalize();
    }
}
