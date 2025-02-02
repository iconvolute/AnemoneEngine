#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"

namespace Anemone::Internal
{
    struct NativeSharedLibrary final
    {
        HMODULE Value{nullptr};

        constexpr bool IsValid() const
        {
            return this->Value != nullptr;
        }

        static constexpr NativeSharedLibrary Invalid()
        {
            return NativeSharedLibrary{};
        }
    };
}
