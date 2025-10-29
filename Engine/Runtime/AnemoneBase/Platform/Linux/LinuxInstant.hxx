#pragma once
#include "AnemoneBase/Instant.hxx"

namespace Anemone
{
    struct LinuxInstant final
    {
        static Instant Now();
    };
}
