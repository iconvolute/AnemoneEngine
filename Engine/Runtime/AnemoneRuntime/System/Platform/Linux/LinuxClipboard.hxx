#pragma once
#include "AnemoneRuntime/Interop/Linux/Headers.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal
{
    struct LinuxClipboardStatics final
    {
    };

    extern UninitializedObject<LinuxClipboardStatics> GClipboardStatics;
}
