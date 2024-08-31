#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Diagnostic
{
    struct DiagnosticStatic final
    {
        Threading::CriticalSection ErrorLock{};
        ErrorReportingMode ReportingMode{ErrorReportingMode::Interactive};
    };

    extern UninitializedObject<DiagnosticStatic> GDiagnosticStatic;

    void InitializeStatic();
    void ShutdownStatic();
}
