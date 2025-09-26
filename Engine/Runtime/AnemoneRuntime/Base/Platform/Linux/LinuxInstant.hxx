#pragma once
#include "AnemoneRuntime/Base/Instant.hxx"

namespace Anemone
{
    struct LinuxInstant final
    {
        static Instant Now();
    };
}
