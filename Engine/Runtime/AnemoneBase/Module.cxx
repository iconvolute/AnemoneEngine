#include "AnemoneBase/Module.hxx"
#include "AnemoneDiagnostics/Trace.hxx"
#include "AnemoneDiagnostics/Debug.hxx"
#include "AnemoneSystem/Environment.hxx"
#include "AnemoneSystem/ProcessorProperties.hxx"
#include "AnemoneStorage/FileSystem.hxx"
#include "AnemoneSystem/Clipboard.hxx"

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
    }

    void Module_Runtime::Finalize()
    {
        Anemone::Clipboard::Finalize();
        Anemone::FileSystem::Finalize();
        Anemone::ProcessorProperties::Finalize();
        Anemone::Environment::Finalize();
        Anemone::Debug::Finalize();
        Anemone::Trace::Finalize();
    }
}
