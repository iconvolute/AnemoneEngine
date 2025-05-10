#include "AnemoneRuntime/System/SharedLibrary.hxx"
#include "AnemoneRuntime/Interop/Windows/FileSystem.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    auto SharedLibrary::Load(std::string_view path) -> std::expected<SharedLibrary, ErrorCode>
    {
        Interop::Windows::FilePathW wPath{};
        Interop::Windows::WidenString(wPath, path);

        Interop::Windows::SafeSharedLibraryHandle h{LoadLibraryW(wPath.c_str())};

        if (h)
        {
            return SharedLibrary{std::move(h)};
        }

        return std::unexpected(ErrorCode::InvalidArgument);
    }

    auto SharedLibrary::GetSymbol(const char* name) const -> std::expected<void*, ErrorCode>
    {
        AE_ASSERT(this->_handle);

        if (this->_handle)
        {
            if (void* const symbol = GetProcAddress(this->_handle.Get(), name))
            {
                return symbol;
            }

            return std::unexpected(ErrorCode::NotFound);
        }

        return std::unexpected(ErrorCode::InvalidHandle);
    }
}
