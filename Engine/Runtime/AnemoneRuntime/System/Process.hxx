#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/Types.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <string_view>
#include <optional>
#include <expected>
#include <string>

namespace Anemone
{
    struct Duration;
}

namespace Anemone::System
{
    class FileHandle;

    class RUNTIME_API Process final
    {
    private:
        Interop::NativeProcess m_native;

    public:
        explicit Process(Interop::NativeProcess native);

    public:
        Process();
        Process(Process&& other) noexcept;
        Process& operator=(Process&& other) noexcept;
        ~Process();

        Process(Process const&) = delete;
        Process& operator=(Process const&) = delete;

    public:
        static std::expected<Process, ErrorCode> Start(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory)
        {
            return Start(
                path,
                params,
                workingDirectory,
                nullptr,
                nullptr,
                nullptr);
        }

        static std::expected<Process, ErrorCode> Start(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory,
            FileHandle* input,
            FileHandle* output,
            FileHandle* error);

        static std::expected<int32_t, ErrorCode> Execute(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory,
            std::string& output,
            std::string& error);

        static std::expected<int32_t, ErrorCode> Execute(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory);

        std::expected<int32_t, ErrorCode> Wait() const;

        std::expected<std::optional<int32_t>, ErrorCode> TryWait() const;

        std::expected<void, ErrorCode> Terminate();
    };
}
