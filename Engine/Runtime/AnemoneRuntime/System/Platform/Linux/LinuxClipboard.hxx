#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Private
{
    struct LinuxClipboardStatics final
    {
    };

    extern UninitializedObject<LinuxClipboardStatics> GClipboardStatics;
}
