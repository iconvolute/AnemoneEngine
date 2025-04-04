#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/FunctionRef.hxx"

#include <string_view>

namespace Anemone
{
    struct StackTrace final
    {
        friend struct Runtime;

    private:
        static void Initialize();
        static void Finalize();

    public:
        StackTrace() = delete;

        static RUNTIME_API void Walk(FunctionRef<void(void* address)> callback);
        static RUNTIME_API void Walk(FunctionRef<void(void* address, std::string_view symbol)> callback);
    };
}
