#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"

namespace Anemone::Internal
{
    struct NativeFileHandle
    {
        HANDLE Value{nullptr};

        constexpr bool IsValid() const
        {
            return this->Value != nullptr;
        }

        static NativeFileHandle Invalid()
        {
            return NativeFileHandle{};
        }
    };
}
