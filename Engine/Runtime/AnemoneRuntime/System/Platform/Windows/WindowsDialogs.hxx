#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal
{
    struct WindowsDialogsStatics final
    {
    };

    extern UninitializedObject<WindowsDialogsStatics> GDialogsStatics;
}
