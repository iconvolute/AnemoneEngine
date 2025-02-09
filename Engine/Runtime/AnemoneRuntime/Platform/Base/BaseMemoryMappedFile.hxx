#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

namespace Anemone
{
    enum class MemoryMappedFileAccess
    {
        ReadOnly,
        ReadWrite,
        CopyOnWrite,
        ReadExecute,
    };
}
