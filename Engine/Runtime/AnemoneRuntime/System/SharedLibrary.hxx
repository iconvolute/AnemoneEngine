#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/Types.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <expected>
#include <string_view>

namespace Anemone::System
{
    class RUNTIME_API SharedLibrary final
    {
    private:
        Interop::NativeSharedLibrary m_native;

    public:
        SharedLibrary(Interop::NativeSharedLibrary const& native);

    public:
        SharedLibrary();
        SharedLibrary(SharedLibrary&& other) noexcept;
        SharedLibrary& operator=(SharedLibrary&& other) noexcept;
        ~SharedLibrary();

        SharedLibrary(SharedLibrary const&) = delete;
        SharedLibrary& operator=(SharedLibrary const&) = delete;

    public:
        static std::expected<SharedLibrary, ErrorCode> Open(std::string_view path);
        std::expected<void, ErrorCode> Close();
        explicit operator bool() const;

        void* GetSymbol(char const* name) const;
    };
}
