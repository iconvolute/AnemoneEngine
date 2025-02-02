#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <string_view>
#include <string>
#include <optional>
#include <expected>

namespace Anemone::Internal
{
    struct NativeProcessHandle final
    {
        HANDLE Value{nullptr};

        constexpr bool IsValid() const
        {
            return this->Value != nullptr;
        }

        static constexpr NativeProcessHandle Invalid()
        {
            return NativeProcessHandle{};
        }
    };
}
