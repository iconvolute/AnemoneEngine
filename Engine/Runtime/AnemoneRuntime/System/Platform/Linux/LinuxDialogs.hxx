#pragma once
#include "AnemoneRuntime/Interop/Linux/Headers.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal
{
    struct LinuxDialogsStatics final
    {
    };

    extern UninitializedObject<LinuxDialogsStatics> GDialogsStatics;
}
