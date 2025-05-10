#pragma once
#include "AnemoneRuntime/System/Platform/Platform.hxx"
#include "AnemoneRuntime/Base/ErrorCode.hxx"

#include <expected>

namespace Anemone
{
    class RUNTIME_API SharedLibrary final
    {
    private:
        Internal::PlatformSharedLibrary _handle{};

    public:
        explicit SharedLibrary(Internal::PlatformSharedLibrary handle)
            : _handle{std::move(handle)}
        {
        }

        SharedLibrary(SharedLibrary const&) = delete;
        SharedLibrary(SharedLibrary&&) noexcept = default;
        SharedLibrary& operator=(SharedLibrary const&) = delete;
        SharedLibrary& operator=(SharedLibrary&&) = default;
        ~SharedLibrary() = default;


    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] Internal::PlatformSharedLibrary const& GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        static auto Load(std::string_view path) -> std::expected<SharedLibrary, ErrorCode>;

    public:
        auto GetSymbol(const char* name) const -> std::expected<void*, ErrorCode>;
    };
}
