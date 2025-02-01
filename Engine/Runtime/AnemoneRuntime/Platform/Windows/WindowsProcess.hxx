#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/ErrorCode.hxx"

#include <string_view>
#include <string>
#include <optional>
#include <expected>

namespace Anemone
{
    class WindowsFileHandle;

    class RUNTIME_API WindowsProcess final
    {
    private:
        HANDLE _handle{};

    public:
        explicit WindowsProcess(HANDLE handle);

        WindowsProcess() = default;

        WindowsProcess(WindowsProcess const&) = delete;

        WindowsProcess(WindowsProcess&& other) noexcept;

        WindowsProcess& operator=(WindowsProcess const&) = delete;

        WindowsProcess& operator=(WindowsProcess&& other) noexcept;

        ~WindowsProcess();

    public:
        static std::expected<WindowsProcess, ErrorCode> Start(
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

        static std::expected<WindowsProcess, ErrorCode> Start(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory,
            WindowsFileHandle* input,
            WindowsFileHandle* output,
            WindowsFileHandle* error);

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
            return this->_handle != nullptr;
        }

        [[nodiscard]] HANDLE GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        std::expected<int32_t, ErrorCode> Wait();

        std::expected<int32_t, ErrorCode> TryWait();

        std::expected<void, ErrorCode> Terminate();
    };

    using Process = WindowsProcess;
}
