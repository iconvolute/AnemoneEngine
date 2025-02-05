#pragma once
#include "AnemoneRuntime/Platform/Unix/UnixHeaders.hxx"
#include "AnemoneRuntime/Platform/Base/BaseProcess.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixSafeHandle.hxx"

namespace Anemone
{
    class LinuxFileHandle;

    class RUNTIME_API LinuxProcess final : public BaseProcess
    {
    private:
        Interop::UnixSafePidHandle _handle{};

    public:
        explicit LinuxProcess(Interop::UnixSafePidHandle handle)
            : _handle{std::move(handle)}
        {
        }

        LinuxProcess() = default;

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
        [[nodiscard]] explicit operator bool() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] Interop::UnixSafePidHandle const& GetNativeHandle() const
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
