#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal
{
    struct LinuxClipboardStatics final
    {
    };

    extern UninitializedObject<LinuxClipboardStatics> GClipboardStatics;
}
