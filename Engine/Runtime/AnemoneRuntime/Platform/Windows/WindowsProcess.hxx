#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseProcess.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsSafeHandle.hxx"

namespace Anemone
{
    class WindowsFileHandle;

    class RUNTIME_API WindowsProcess final : public BaseProcess
    {
    private:
        Interop::Win32SafeHandle _handle{};

    public:
        explicit WindowsProcess(Interop::Win32SafeHandle handle)
            : _handle{std::move(handle)}
        {
        }

        WindowsProcess() = default;

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
        [[nodiscard]] explicit operator bool() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] Interop::Win32SafeHandle const& GetNativeHandle() const
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
