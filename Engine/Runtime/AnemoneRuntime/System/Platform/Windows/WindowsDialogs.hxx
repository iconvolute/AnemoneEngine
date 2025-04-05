#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Private
{
    struct WindowsDialogsStatics final
    {
    };

    extern UninitializedObject<WindowsDialogsStatics> GDialogsStatics;
}
