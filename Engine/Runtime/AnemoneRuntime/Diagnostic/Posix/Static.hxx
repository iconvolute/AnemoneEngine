#pragma once
#include "AnemoneRuntime/Diagnostic/Static.hxx"

namespace Anemone::Diagnostic
{
    struct PosixDiagnosticStatic : GenericDiagnosticStatic
    {
        static void Initialize();
        static void Finalize();
    };

    using DiagnosticStatic = PosixDiagnosticStatic;
}
