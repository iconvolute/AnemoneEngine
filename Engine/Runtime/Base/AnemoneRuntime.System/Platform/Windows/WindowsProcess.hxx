#pragma once
#include "AnemoneRuntime.System/Process.hxx"
#include "AnemoneRuntime.Interop/Windows/SafeHandle.hxx"

namespace Anemone
{
    class WindowsProcess final : public Process
    {
    private:
        Interop::Windows::SafeHandle _handle{};

    public:
        WindowsProcess();

        explicit WindowsProcess(
            Interop::Windows::SafeHandle handle,
            Reference<StreamReader> standardOutputReader,
            Reference<StreamReader> standardErrorReader,
            Reference<StreamWriter> standardInputWriter);

        WindowsProcess(WindowsProcess const&) = delete;

        WindowsProcess(WindowsProcess&&) noexcept = delete;

        ~WindowsProcess() override;

        WindowsProcess& operator=(WindowsProcess const&) = delete;

        WindowsProcess& operator=(WindowsProcess&&) noexcept = delete;

    public:
        std::expected<int32_t, Error> Wait() override;

        std::expected<int32_t, Error> TryWait(Duration timeout) override;

        std::expected<int32_t, Error> TryWait() override;

    public:
        static Reference<Process> Start(ProcessStartInfo const& processStartInfo);
    };
}
