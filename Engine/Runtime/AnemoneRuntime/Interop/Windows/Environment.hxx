#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"

#include <ShlObj.h>

namespace Anemone::Interop::Windows
{
    template <size_t CapacityT>
    anemone_forceinline bool GetCurrentDirectory(string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
        {
            // MSDN:
            //  If the function succeeds, the return value specifies the number of characters that are written to the
            //  buffer, not including the terminating null character.

            if (DWORD dwLength = ::GetCurrentDirectoryW(static_cast<DWORD>(buffer.size() + 1uz), buffer.data()); dwLength != 0)
            {
                if (dwLength > buffer.size())
                {
                    // MSDN:
                    //  If the buffer that is pointed to by lpBuffer is not large enough, the return value specifies
                    //  the required size of the buffer, in characters, including the null-terminating character.
                    --dwLength;
                }
                capacity = static_cast<size_t>(dwLength);
                return true;
            }

            capacity = 0;
            return false;
        });
    }
    
    template <size_t CapacityT>
    anemone_forceinline bool GetEnvironmentVariable(string_buffer<wchar_t, CapacityT>& result, const wchar_t* name) noexcept
    {
        return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
        {
            // MSDN:
            //  The size of the buffer pointed to by the lpBuffer parameter, including the null-terminating character,
            //  in characters.

            SetLastError(ERROR_SUCCESS);
            DWORD dwLength = GetEnvironmentVariableW(name, buffer.data(), static_cast<DWORD>(buffer.size() + 1uz));

            if ((dwLength == 0) and (GetLastError() != ERROR_SUCCESS))
            {
                // MSDN:
                //  If the function fails, the return value is zero.
                capacity = 0;
                return false;
            }

            if (dwLength > buffer.size())
            {
                // MSDN:
                //  If lpBuffer is not large enough to hold the data, the return value is the buffer size, in
                //  characters, required to hold the string and its terminating null character and the contents of
                //  lpBuffer are undefined.
                --dwLength;
            }

            capacity = dwLength;
            return true;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool GetSystemDirectory(string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
        {
            UINT length = GetSystemDirectoryW(buffer.data(), static_cast<UINT>(buffer.size() + 1uz));

            if (length == 0)
            {
                capacity = 0;
                return false;
            }

            // MSDN:
            //  If the function succeeds, the return value is the length, in TCHARs, of the string copied to the
            //  buffer, not including the terminating null character.

            if (length > buffer.size())
            {
                // MSDN:
                //  If the length is greater than the size of the buffer, the return value is the size of the buffer
                //  required to hold the path, including the terminating null character.
                --length;
            }

            capacity = length;
            return true;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool GetUserDefaultLocaleName(string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        std::span<wchar_t> writable = result.as_buffer_span();
        if (int const length = ::GetUserDefaultLocaleName(writable.data(), static_cast<int>(writable.size())); length > 0)
        {
            result.trim(static_cast<size_t>(length - 1));
            return true;
        }

        result.trim(0);
        return false;
    }

    template <size_t CapacityT>
    anemone_forceinline bool GetComputerName(string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
        {
            // MSDN:
            //  On input, specifies the size of the buffer, in TCHARs.
            DWORD dwSize = static_cast<DWORD>(buffer.size() + 1uz);

            if (GetComputerNameW(buffer.data(), &dwSize))
            {
                // MSDN:
                //  On output, the number of TCHARs copied to the destination buffer, not including the terminating null
                //  character.
                capacity = static_cast<size_t>(dwSize);
                return true;
            }

            if (GetLastError() == ERROR_BUFFER_OVERFLOW)
            {
                // MSDN:
                //  If the buffer is too small, the function fails and GetLastError returns ERROR_BUFFER_OVERFLOW.
                //  The lpnSize parameter specifies the size of the buffer required, including the terminating null
                //  character.
                capacity = dwSize - 1u;
                return true;
            }

            capacity = 0;
            return false;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool GetComputerNameEx(COMPUTER_NAME_FORMAT format, string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
        {
            // MSDN:
            //  On input, specifies the size of the buffer, in TCHARs.

            DWORD dwSize = static_cast<DWORD>(buffer.size() + 1uz);
            if (GetComputerNameExW(format, buffer.data(), &dwSize))
            {
                // MSDN:
                //  On output, receives the number of TCHARs copied to the destination buffer, not including the
                //  terminating null character.
                capacity = static_cast<size_t>(dwSize);
                return true;
            }

            if (GetLastError() == ERROR_MORE_DATA)
            {
                // MSDN:
                //  If the buffer is too small, the function fails and GetLastError returns ERROR_MORE_DATA.
                //  This parameter receives the size of the buffer required, including the terminating null character.
                capacity = dwSize - 1uz;
                return true;
            }

            capacity = 0;
            return false;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool GetUserName(string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
        {
            DWORD dwSize = static_cast<DWORD>(buffer.size() + 1uz);
            if (GetUserNameW(buffer.data(), &dwSize))
            {
                // MSDN:
                //  On output, the variable receives the number of TCHARs copied to the buffer, including the
                //  terminating null character.
                capacity = dwSize - 1uz;
                return true;
            }

            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                // MSDN:
                //  If lpBuffer is too small, the function fails and GetLastError returns ERROR_INSUFFICIENT_BUFFER.
                //  This parameter receives the required buffer size, including the terminating null character.
                capacity = dwSize - 1uz;
                return true;
            }

            capacity = 0;
            return false;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool GetUserPreferredUILanguages(string_buffer<wchar_t, CapacityT>& result, DWORD flags, ULONG& numLanguages)
    {
        // TODO: This API should be replaced with proper splitting as well.
        ULONG uSize = 0;

        if (::GetUserPreferredUILanguages(flags, &numLanguages, nullptr, &uSize))
        {
            result.resize_for_override(uSize - 1uz);

            if (::GetUserPreferredUILanguages(flags, &numLanguages, result.data(), &uSize))
            {
                // Trim to result.
                result.trim(uSize - 1uz);
                return true;
            }
        }

        return false;
    }

    template <typename CallbackT = void(std::wstring_view)>
    anemone_forceinline bool GetKnownFolderPath(KNOWNFOLDERID const& id, CallbackT callback) noexcept
    {
        PWSTR result{};

        if (HRESULT const hr = SHGetKnownFolderPath(id, 0, nullptr, &result); SUCCEEDED(hr))
        {
            if (result != nullptr)
            {
                callback(std::wstring_view{result});
                CoTaskMemFree(result);
                return true;
            }
        }

        return false;
    }
}
