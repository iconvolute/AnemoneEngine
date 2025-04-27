#pragma once
#include "AnemoneRuntime/UninitializedObject.hxx"

namespace Anemone::Internal::Linux
{
    struct ApplicationStatics final
    {
    };
}

namespace Anemone::Internal
{
    extern UninitializedObject<Linux::ApplicationStatics> GApplicationStatics;
}
