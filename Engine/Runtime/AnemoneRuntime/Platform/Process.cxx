#include "AnemoneRuntime/Platform/Process.hxx"
#include "AnemoneRuntime/Platform/FileHandle.hxx"

#include <array>

namespace Anemone
{
    std::expected<int32_t, ErrorCode> Process::Execute(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory)
    {
        auto sc = Start(path, params, workingDirectory, nullptr, nullptr, nullptr).and_then([](Process process)
        {
            return process.Wait();
        });
        return sc;
    }

    std::expected<int32_t, ErrorCode> Process::Execute(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory,
        std::string& output,
        std::string& error)
    {
        // TODO: This code is common to all supported platforms.
        FileHandle pipeOutput{};
        FileHandle pipeError{};

        if (auto process = Start(path, params, workingDirectory, nullptr, &pipeOutput, &pipeError))
        {
            std::array<char, 1024> buffer;

            for (;;)
            {
                auto ret = process->TryWait();

                while (auto processed = pipeOutput.Read(std::as_writable_bytes(std::span{buffer})))
                {
                    if (*processed == 0)
                    {
                        break;
                    }

                    output.append(reinterpret_cast<char const*>(buffer.data()), *processed);
                }

                while (auto processed = pipeError.Read(std::as_writable_bytes(std::span{buffer})))
                {
                    if (*processed == 0)
                    {
                        break;
                    }

                    error.append(reinterpret_cast<char const*>(buffer.data()), *processed);
                }

                if (ret)
                {
                    return *ret;
                }

                if (ret.error() != ErrorCode::OperationInProgress)
                {
                    return std::unexpected(ret.error());
                }
            }
        }
        else
        {
            return std::unexpected(process.error());
        }
    }
}
