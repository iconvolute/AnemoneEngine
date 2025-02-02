#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"

namespace Anemone::Internal
{
    struct NativeNamedMutex final
    {
        HANDLE Value{nullptr};

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
