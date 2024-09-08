#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <optional>

namespace Anemone::Network
{
    RUNTIME_API std::optional<bool> HasInternetConnection();
}
