#include "AnemoneRuntime/Platform/Process.hxx"
#include "AnemoneRuntime/Platform/FileHandle.hxx"

#include <span>
#include <array>

namespace Anemone
{
    auto BaseProcess::Execute(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory)
        -> std::expected<int32_t, ErrorCode>
    {
        auto sc = Process::Start(path, params, workingDirectory, nullptr, nullptr, nullptr).and_then([](Process process)
        {
            return process.Wait();
        });
        return sc;
    }

    auto BaseProcess::Execute(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory,
        FunctionRef<void(std::string_view)> output,
        FunctionRef<void(std::string_view)> error)
        -> std::expected<int32_t, ErrorCode>
    {
        FileHandle pipeOutput{};
        FileHandle pipeError{};

        if (auto process = Process::Start(path, params, workingDirectory, nullptr, &pipeOutput, &pipeError))
        {
            std::array<char, 512> buffer;
            std::span view = std::as_writable_bytes(std::span{buffer});

            while (true)
            {
                auto rc = process->TryWait();

                while (auto processed = pipeOutput.Read(view))
                {
                    if (*processed == 0)
                    {
                        break;
                    }

                    output(std::string_view{buffer.data(), *processed});
                }

                while (auto processed = pipeError.Read(view))
                {
                    if (*processed == 0)
                    {
                        break;
                    }

                    error(std::string_view{buffer.data(), *processed});
                }

                if (rc)
                {
                    return *rc;
                }

                if (rc.error() != ErrorCode::OperationInProgress)
                {
                    return std::unexpected(rc.error());
                }
            }
        }
        else
        {
            return std::unexpected(process.error());
        }
    }

    auto BaseProcess::Execute(
        std::string_view path,
        std::optional<std::string_view> const& params,
        std::optional<std::string_view> const& workingDirectory,
        std::string& output,
        std::string& error)
        -> std::expected<int32_t, ErrorCode>
    {
        return Process::Execute(
            path,
            params,
            workingDirectory,
            [&output](std::string_view s)
        {
            output.append(s);
        },
            [&error](std::string_view s)
        {
            error.append(s);
        });
    }
}
