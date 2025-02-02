#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"

namespace Anemone::Internal
{
    struct NativeNamedMutex final
    {
        sem_t* Value{nullptr};

        constexpr bool IsValid() const
        {
            return this->Value != nullptr;
        }

        static NativeNamedMutex Invalid()
        {
            return NativeNamedMutex{};
        }
    };
}
