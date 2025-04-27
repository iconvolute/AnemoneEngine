#pragma once
#include "AnemoneRuntime/Interop/Linux/SafeHandle.hxx"

namespace Anemone::Internal
{
    using PlatformFileHandle = Interop::Linux::SafeFdHandle;
}
