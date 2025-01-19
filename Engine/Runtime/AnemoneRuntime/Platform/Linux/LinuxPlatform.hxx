#pragma once
#include "AnemoneRuntime/Platform/Platform.hxx"

namespace Anemone::Platform
{
    
}

namespace Anemone::Platform::Internal
{
    struct LinuxPlatformStatics final
    {
    };

    RUNTIME_API extern UninitializedObject<LinuxPlatformStatics> GLinuxPlatformStatics;
}
