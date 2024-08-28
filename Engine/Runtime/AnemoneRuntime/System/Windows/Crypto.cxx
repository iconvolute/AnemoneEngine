#include "AnemoneRuntime/System/Crypto.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <wincrypt.h>

ANEMONE_EXTERNAL_HEADERS_END

namespace Anemone::System
{
    bool GetRandomBytes(std::span<std::byte> buffer)
    {
        if (not buffer.empty())
        {
            NTSTATUS status = BCryptGenRandom(
                nullptr,
                reinterpret_cast<UCHAR*>(buffer.data()),
                static_cast<ULONG>(buffer.size()),
                BCRYPT_USE_SYSTEM_PREFERRED_RNG);

            return SUCCEEDED(status);
        }

        return false;
    }
}
