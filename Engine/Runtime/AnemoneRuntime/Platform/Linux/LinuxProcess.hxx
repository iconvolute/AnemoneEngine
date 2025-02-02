#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"

namespace Anemone::Internal
{
    struct NativeProcessHandle final
    {
        pid_t Value{-1};

        constexpr bool IsValid() const
        {
            return this->Value > 0;
        }

        static constexpr NativeProcessHandle Invalid()
        {
            return NativeProcessHandle{};
        }
    };
}
