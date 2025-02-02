#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

namespace Anemone::Internal
{
    struct NativeSharedLibrary final
    {
        void* Value{nullptr};

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
