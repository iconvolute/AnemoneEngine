#pragma once
#include <cstdint>

namespace Anemone::VM
{
    struct OpCodeDefinition
    {
        uint32_t Value;
        uint8_t Size;
    };
}
