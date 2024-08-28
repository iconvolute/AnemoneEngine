#include "AnemoneRuntime/System/Crypto.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <sys/random.h>

ANEMONE_EXTERNAL_HEADERS_END

namespace Anemone::System
{
    bool GetRandomBytes(std::span<std::byte> buffer)
    {
        if (not buffer.empty())
        {
            ssize_t size = getrandom(
                buffer.data(),
                buffer.size(),
                GRND_RANDOM);

            if (size < 0)
            {
                return false;
            }

            if (static_cast<size_t>(size) == buffer.size())
            {
                return true;
            }

            return true;
        }

        return false;
    }
}
