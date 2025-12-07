#pragma once
#include "AnemoneRuntime.Base/Reference.hxx"
#include "AnemoneRuntime.Base/FunctionRef.hxx"
#include "AnemoneRuntime.Base/Duration.hxx"
#include "AnemoneRuntime.Storage/FileHandle.hxx"
#include "AnemoneRuntime.Storage/StreamReader.hxx"
#include "AnemoneRuntime.Storage/StreamWriter.hxx"

#include <string_view>
#include <optional>

namespace Anemone
{
    struct ProcessStartInfo final
    {
        std::string FileName{};

        std::optional<std::string> StartupDirectory = std::nullopt;

        std::vector<std::string> Parameters{};

        bool RedirectStandardInput = false;
        bool RedirectStandardOutput = false;
        bool RedirectStandardError = false;
    };

    class Process : public ReferenceCounted<Process>
    {
    protected:
        Reference<StreamReader> _standardOutputReader{};
        Reference<StreamReader> _standardErrorReader{};
        Reference<StreamWriter> _standardInputWriter{};

    protected:
        Process();

    public:
        Process(Process const&) = delete;

        Process(Process&&) noexcept = delete;

        virtual ~Process();

        Process& operator=(Process const&) = delete;

        Process& operator=(Process&&) noexcept = delete;

    public:
        virtual std::expected<int32_t, Error> Wait() = 0;

        virtual std::expected<int32_t, Error> TryWait(Duration timeout) = 0;

        virtual std::expected<int32_t, Error> TryWait() = 0;

        [[nodiscard]] Reference<StreamReader> const& GetStandardOutputReader() const
        {
            return this->_standardOutputReader;
        }

        [[nodiscard]] Reference<StreamReader> const& GetStandardErrorReader() const
        {
            return this->_standardErrorReader;
        }

        [[nodiscard]] Reference<StreamWriter> const& GetStandardInputWriter() const
        {
            return this->_standardInputWriter;
        }

    public:
        ANEMONE_RUNTIME_BASE_API static Reference<Process> Start(ProcessStartInfo const& processStartInfo);
    };
}
