#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

namespace Anemone::Internal
{
    struct NativeFileHandle
    {
        int Value{-1};

        constexpr bool IsValid() const
        {
            return this->Value >= 0;
        }

        static NativeFileHandle Invalid()
        {
            return NativeFileHandle{};
        }
    };
}
