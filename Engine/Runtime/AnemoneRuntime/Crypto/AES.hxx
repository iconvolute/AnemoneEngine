#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

namespace Anemone::Crypto
{
    struct AESContext
    {
        size_t Rounds{};
        uint32_t EncryptionKey[60];
        uint32_t DecryptionKey[60];
    };

    bool RUNTIME_API Initialize(AESContext& self, const uint8_t* key, size_t length);

    void RUNTIME_API Encrypt(AESContext const& self, const uint8_t* input, uint8_t* output);

    void RUNTIME_API Decrypt(AESContext const& self, const uint8_t* input, uint8_t* output);
}
