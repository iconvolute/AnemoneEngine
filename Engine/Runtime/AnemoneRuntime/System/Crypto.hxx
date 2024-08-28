#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <span>
#include <cstddef>

namespace Anemone::System
{
    RUNTIME_API bool GetRandomBytes(std::span<std::byte> buffer);
}
