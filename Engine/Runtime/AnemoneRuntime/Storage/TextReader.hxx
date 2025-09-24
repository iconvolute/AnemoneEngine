#pragma once
#include "AnemoneRuntime/Base/Reference.hxx"
#include "AnemoneRuntime/Diagnostics/Error.hxx"

#include <expected>
#include <span>
#include <string>

namespace Anemone
{
    class RUNTIME_API TextReader : public ReferenceCounted<TextReader>
    {
    public:
        TextReader();

        TextReader(TextReader const&) = delete;

        TextReader(TextReader&&) = delete;

        virtual ~TextReader();

        TextReader& operator=(TextReader const&) = delete;

        TextReader& operator=(TextReader&&) = delete;

    public:
        virtual std::expected<size_t, Error> Read(std::span<char> buffer) = 0;

        virtual std::expected<std::string, Error> ReadLine() = 0;

        virtual std::expected<std::string, Error> ReadToEnd() = 0;

        virtual std::expected<char, Error> Read() = 0;

        virtual std::expected<char, Error> Peek() = 0;
    };
}
