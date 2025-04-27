#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal
{
    struct WindowsDialogsStatics final
    {
    };

    extern UninitializedObject<WindowsDialogsStatics> GDialogsStatics;
}
