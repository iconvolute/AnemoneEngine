#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <string_view>
#include <string>
#include <optional>
#include <expected>

namespace Anemone
{
    class LinuxFileHandle;

    class RUNTIME_API LinuxProcess final
    {
    private:
        pid_t _handle{};

    public:
        explicit LinuxProcess(pid_t handle);

        LinuxProcess() = default;

        LinuxProcess(LinuxProcess const&) = delete;

        LinuxProcess(LinuxProcess&& other) noexcept;

        LinuxProcess& operator=(LinuxProcess const&) = delete;

        LinuxProcess& operator=(LinuxProcess&& other) noexcept;

        ~LinuxProcess();

    public:
        static std::expected<LinuxProcess, ErrorCode> Start(
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

        static std::expected<LinuxProcess, ErrorCode> Start(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory,
            LinuxFileHandle* input,
            LinuxFileHandle* output,
            LinuxFileHandle* error);

    public:
        static std::expected<int32_t, ErrorCode> Execute(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory);

        static std::expected<int32_t, ErrorCode> Execute(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory,
            std::string& output,
            std::string& error);

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle >= 0;
        }

        [[nodiscard]] pid_t GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        std::expected<int32_t, ErrorCode> Wait();

        std::expected<int32_t, ErrorCode> TryWait();

        std::expected<void, ErrorCode> Terminate();
    };

    using Process = LinuxProcess;
}
