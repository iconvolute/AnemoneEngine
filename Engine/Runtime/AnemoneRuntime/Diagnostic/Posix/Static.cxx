#include "AnemoneRuntime/Diagnostic/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Log.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostic/StandardOutputLogListener.hxx"

namespace Anemone::Diagnostic
{
    static UninitializedObject<StandardOutputLogListener> GStandardOutputLogListener{};

    void InitializeStatic()
    {
        GStandardOutputLogListener.Create();
    }

    void ShutdownStatic()
    {
        GStandardOutputLogListener.Destroy();
    }
}
