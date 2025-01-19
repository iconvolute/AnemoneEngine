#pragma once
#include "AnemoneRuntime/Platform/Platform.hxx"

namespace Anemone::Platform
{

}

namespace Anemone::Platform::Internal
{
    struct AndroidPlatformStatics final
    {
    };

    RUNTIME_API extern UninitializedObject<AndroidPlatformStatics> GAndroidPlatformStatics;
}
