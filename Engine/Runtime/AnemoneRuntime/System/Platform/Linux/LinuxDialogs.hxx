#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal
{
    struct LinuxDialogsStatics final
    {
    };

    extern UninitializedObject<LinuxDialogsStatics> GDialogsStatics;
}
