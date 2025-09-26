#pragma once
#include "AnemoneRuntime/Base/Instant.hxx"

namespace Anemone
{
    struct WindowsInstant final
    {
        static int64_t QueryInstantFrequency();
        static int64_t QueryInstantValue();

        static Instant Now();
    };
}
