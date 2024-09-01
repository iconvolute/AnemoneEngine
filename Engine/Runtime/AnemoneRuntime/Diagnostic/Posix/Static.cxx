#include "AnemoneRuntime/Diagnostic/Static.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Diagnostic
{
    void PosixDiagnosticStatic::Initialize()
    {
        GenericDiagnosticStatic::Initialize();
    }

    void PosixDiagnosticStatic::Finalize()
    {
        GenericDiagnosticStatic::Finalize();
    }
}
