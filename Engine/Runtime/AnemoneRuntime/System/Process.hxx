#pragma once
#include "AnemoneRuntime/System/Platform/Platform.hxx"
#include "AnemoneRuntime/System/FileHandle.hxx"
#include "AnemoneRuntime/Base/FunctionRef.hxx"
#include "AnemoneRuntime/Diagnostics/Status.hxx"

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
        std::expected<int32_t, Status> Wait();

        std::expected<int32_t, Status> TryWait();

        std::expected<void, Status> Terminate();

    public:
        static auto Start(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory)
            -> std::expected<Process, Status>;

        static auto Start(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory,
            FileHandle* input,
            FileHandle* output,
            FileHandle* error)
            -> std::expected<Process, Status>;

        static auto Execute(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory)
            -> std::expected<int32_t, Status>;

        static auto Execute(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory,
            FunctionRef<void(std::string_view)> output,
            FunctionRef<void(std::string_view)> error)
            -> std::expected<int32_t, Status>;
    };
}
