#pragma once
#include "AnemoneBase/Reference.hxx"
#include "AnemoneDiagnostics/Error.hxx"

#include <expected>

namespace Anemone
{
    class SharedLibrary : public ReferenceCounted<SharedLibrary>
    {
    protected:
        SharedLibrary();

    public:
        SharedLibrary(SharedLibrary const&) = delete;

        SharedLibrary(SharedLibrary&&) = delete;

        virtual ~SharedLibrary();

        SharedLibrary& operator=(SharedLibrary const&) = delete;

        SharedLibrary& operator=(SharedLibrary&&) = delete;

    public:
        RUNTIME_API static std::expected<Reference<SharedLibrary>, Error> Load(std::string_view path);

    public:
        virtual std::expected<void*, Error> GetSymbol(const char* name) const = 0;
    };
}
