#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Private
{
    struct LinuxDialogsStatics final
    {
    };

    extern UninitializedObject<LinuxDialogsStatics> GDialogsStatics;
}
