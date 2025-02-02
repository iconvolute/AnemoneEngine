#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/WindowsProcess.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Platform/Linux/LinuxProcess.hxx"
#elif ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Android/AndroidProcess.hxx"
#else
#error "Unsupported platform"
#endif
#pragma once

#include "AnemoneRuntime/ErrorCode.hxx"
#include "AnemoneRuntime/FunctionRef.hxx"
#include "AnemoneRuntime/Duration.hxx"

#include <string_view>
#include <string>
#include <optional>
#include <expected>

namespace Anemone
{
    class FileHandle;

    class RUNTIME_API Process final
    {
    private:
        Internal::NativeProcessHandle _handle = Internal::NativeProcessHandle::Invalid();

    public:
        explicit Process(Internal::NativeProcessHandle handle);

        Process() = default;

        Process(Process const&) = delete;

        Process(Process&& other) noexcept;

        Process& operator=(Process const&) = delete;

        Process& operator=(Process&& other) noexcept;

        ~Process();

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

        static std::expected<int32_t, ErrorCode> Execute(
            std::string_view path,
            std::optional<std::string_view> const& params,
            std::optional<std::string_view> const& workingDirectory,
            FunctionRef<void(std::string_view)> output,
            FunctionRef<void(std::string_view)> error);

    public:
        [[nodiscard]] explicit operator bool() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_handle.IsValid();
        }

        [[nodiscard]] Internal::NativeProcessHandle GetNativeHandle() const
        {
            return this->_handle;
        }

    public:
        std::expected<int32_t, ErrorCode> Wait();

        std::expected<int32_t, ErrorCode> TryWait();

        std::expected<int32_t, ErrorCode> TryWait(Duration timeout);

        std::expected<void, ErrorCode> Terminate();
    };
}
