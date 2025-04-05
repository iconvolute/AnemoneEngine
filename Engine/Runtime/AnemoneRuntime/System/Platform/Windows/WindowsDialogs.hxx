#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Private
{
    struct WindowsDialogsStatics final
    {
    };

    extern UninitializedObject<WindowsDialogsStatics> GDialogsStatics;
}
