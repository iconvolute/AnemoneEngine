#include "AnemoneDiagnostics/Trace.hxx"
#include "AnemoneDiagnostics/Debug.hxx"
#include "AnemoneSystem/Environment.hxx"
#include "AnemoneSystem/ProcessorProperties.hxx"
#include "AnemoneStorage/FileSystem.hxx"
#include "AnemoneSystem/Clipboard.hxx"

RUNTIME_API extern void AnemoneRuntime_ModuleInitialize()
{
    Anemone::Trace::Initialize();
    Anemone::Debug::Initialize();
    Anemone::Environment::Initialize();
    Anemone::ProcessorProperties::Initialize();
    Anemone::FileSystem::Initialize();
    Anemone::Clipboard::Initialize();
}

RUNTIME_API extern void AnemoneRuntime_ModuleFinalize()
{
    Anemone::Clipboard::Finalize();
    Anemone::FileSystem::Finalize();
    Anemone::ProcessorProperties::Finalize();
    Anemone::Environment::Finalize();
    Anemone::Debug::Finalize();
    Anemone::Trace::Finalize();
}
