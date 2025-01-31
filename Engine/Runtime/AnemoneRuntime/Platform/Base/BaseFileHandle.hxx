#pragma once
#include <cstdint>

namespace Anemone
{
    enum class FileMode : uint32_t
    {
        // Creates new file. Truncate if exists.
        CreateAlways,

        // Create new file. Fail if exists.
        CreateNew,

        // Create new file. Open if exists.
        OpenAlways,

        // Open if exists. Fail otherwise.
        OpenExisting,

        // Truncates existing file. Fails otherwise.
        TruncateExisting,
    };

    enum class FileAccess : uint32_t
    {
        Read = 1u << 0u,
        Write = 1u << 1u,
        ReadWrite = Read | Write,
    };

    enum class FileOptions : uint32_t
    {
        None = 0u,
        ShareRead = 1u << 0u,
        ShareWrite = 1u << 1u,
        Inheritable = 1u << 2u, // Is it really needed?
        DeleteOnClose = 1u << 3u,
        RandomAccess = 1u << 4u,
        SequentialScan = 1u << 5u,
        WriteThrough = 1u << 6u,
        NoBuffering = 1u << 7u,
        ShareDelete = 1u << 8u,
    };
}
