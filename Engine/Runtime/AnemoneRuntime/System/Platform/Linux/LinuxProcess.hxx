#pragma once
#include "AnemoneRuntime/System/Process.hxx"
#include "AnemoneRuntime/Interop/Linux/SafeHandle.hxx"

namespace Anemone
{
    class LinuxProcess final : public Process
    {
    private:
        Interop::Linux::SafePidFdHandle _handle{};

    public:
        LinuxProcess() = default;

        explicit LinuxProcess(
            Interop::Linux::SafePidFdHandle handle,
            Reference<StreamReader> standardOutputReader,
            Reference<StreamReader> standardErrorReader,
            Reference<StreamWriter> standardInputWriter);

        LinuxProcess(LinuxProcess const&) = delete;

        LinuxProcess(LinuxProcess&&) noexcept = delete;

        ~LinuxProcess() override = default;

        LinuxProcess& operator=(LinuxProcess const&) = delete;

        LinuxProcess& operator=(LinuxProcess&&) noexcept = delete;

    public:
        std::expected<int32_t, Error> Wait() override;

        std::expected<int32_t, Error> TryWait(Duration timeout) override;

        std::expected<int32_t, Error> TryWait() override;

    public:
        static Reference<Process> Start(ProcessStartInfo const& processStartInfo);
    };
}
