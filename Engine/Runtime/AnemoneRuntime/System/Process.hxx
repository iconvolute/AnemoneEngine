#pragma once
#include "AnemoneRuntime/System/Platform/Process.hxx"
#include "AnemoneRuntime/System/FileHandle.hxx"
#include "AnemoneRuntime/FunctionRef.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <string_view>
#include <expected>
#include <optional>

namespace Anemone
{
    class RUNTIME_API Process final
    {
    private:
        Internal::PlatformProcess _handle;

    public:
        Process() = default;
        Process(Process const&) = delete;
        Process(Process&&) noexcept = default;
        Process& operator=(Process const&) = delete;
        Process& operator=(Process&&) noexcept = default;
        ~Process() = default;

        explicit Process(Internal::PlatformProcess handle)
            : _handle{std::move(handle)}
        {
        }

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] Internal::PlatformProcess const& GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        std::expected<int32_t, ErrorCode> Wait();

        std::expected<int32_t, ErrorCode> TryWait();

        std::expected<void, ErrorCode> Terminate();

    public:
        static auto Start(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory)
            -> std::expected<Process, ErrorCode>;

        static auto Start(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory,
            FileHandle* input,
            FileHandle* output,
            FileHandle* error)
            -> std::expected<Process, ErrorCode>;

        static auto Execute(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory)
            -> std::expected<int32_t, ErrorCode>;

        static auto Execute(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory,
            FunctionRef<void(std::string_view)> output,
            FunctionRef<void(std::string_view)> error)
            -> std::expected<int32_t, ErrorCode>;
    };
}
