#include "AnemoneRuntime/Storage/BinaryFormat.hxx"

namespace Anemone::Storage
{
    static_assert(sizeof(BinarySignature) == 8);

    static_assert(sizeof(BinaryFormatVersion) == 8);

    static_assert(sizeof(BinaryBlockHeader) == 24);

    static_assert(sizeof(BinaryFormatHeader) == 24);
}
