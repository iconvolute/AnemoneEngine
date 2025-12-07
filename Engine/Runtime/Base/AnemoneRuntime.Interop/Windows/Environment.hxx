#pragma once
#include "AnemoneRuntime.Interop/Windows/Headers.hxx"
#include "AnemoneRuntime.Interop/Windows/StringBuffer.hxx"
#include "AnemoneRuntime.Interop/Windows/MemoryBuffer.hxx"
#include "AnemoneRuntime.Interop/Windows/SafeHandle.hxx"

#include <ShlObj.h>

namespace Anemone::Interop::Windows
{
    template <size_t Capacity>
    anemone_forceinline HRESULT GetCurrentDirectory(string_buffer<wchar_t, Capacity>& result) noexcept
    {
        return AdaptStringBuffer(result, [](std::span<wchar_t> buffer, size_t& capacity) -> HRESULT
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
                return S_OK;
            }

            capacity = 0;
            return HRESULT_FROM_WIN32(GetLastError());
        });
    }

    template <size_t Capacity>
    anemone_forceinline HRESULT GetEnvironmentVariable(string_buffer<wchar_t, Capacity>& result, const wchar_t* name) noexcept
    {
        return AdaptStringBuffer(result, [&](std::span<wchar_t> buffer, size_t& capacity) -> HRESULT
        {
            // MSDN:
            //  The size of the buffer pointed to by the lpBuffer parameter, including the null-terminating character,
            //  in characters.

            SetLastError(ERROR_SUCCESS);
            DWORD dwLength = GetEnvironmentVariableW(name, buffer.data(), static_cast<DWORD>(buffer.size() + 1uz));

            DWORD dwError = GetLastError();

            if ((dwLength == 0) and (dwError != ERROR_SUCCESS))
            {
                // MSDN:
                //  If the function fails, the return value is zero.
                capacity = 0;
                return HRESULT_FROM_WIN32(dwError);
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
            return S_OK;
        });
    }

    template <size_t Capacity>
    anemone_forceinline HRESULT GetSystemDirectory(string_buffer<wchar_t, Capacity>& result) noexcept
    {
        return AdaptStringBuffer(result, [](std::span<wchar_t> buffer, size_t& capacity) -> HRESULT
        {
            UINT length = GetSystemDirectoryW(buffer.data(), static_cast<UINT>(buffer.size() + 1uz));

            if (length == 0)
            {
                capacity = 0;
                return HRESULT_FROM_WIN32(GetLastError());
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
            return S_OK;
        });
    }

    template <size_t Capacity>
    anemone_forceinline HRESULT GetUserDefaultLocaleName(string_buffer<wchar_t, Capacity>& result) noexcept
    {
        std::span<wchar_t> writable = result.as_buffer_span();
        if (int const length = ::GetUserDefaultLocaleName(writable.data(), static_cast<int>(writable.size())); length > 0)
        {
            result.trim(static_cast<size_t>(length - 1));
            return S_OK;
        }

        result.trim(0);
        return HRESULT_FROM_WIN32(GetLastError());
    }

    template <size_t Capacity>
    anemone_forceinline HRESULT GetComputerName(string_buffer<wchar_t, Capacity>& result) noexcept
    {
        return AdaptStringBuffer(result, [](std::span<wchar_t> buffer, size_t& capacity) -> HRESULT
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
                return S_OK;
            }

            DWORD dwError = GetLastError();
            if (dwError == ERROR_BUFFER_OVERFLOW)
            {
                // MSDN:
                //  If the buffer is too small, the function fails and GetLastError returns ERROR_BUFFER_OVERFLOW.
                //  The lpnSize parameter specifies the size of the buffer required, including the terminating null
                //  character.
                capacity = dwSize - 1u;
                return S_OK;
            }

            capacity = 0;
            return HRESULT_FROM_WIN32(dwError);
        });
    }

    template <size_t Capacity>
    anemone_forceinline HRESULT GetComputerNameEx(COMPUTER_NAME_FORMAT format, string_buffer<wchar_t, Capacity>& result) noexcept
    {
        return AdaptStringBuffer(result, [&](std::span<wchar_t> buffer, size_t& capacity) -> HRESULT
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
                return S_OK;
            }

            DWORD dwError = GetLastError();
            if (dwError == ERROR_MORE_DATA)
            {
                // MSDN:
                //  If the buffer is too small, the function fails and GetLastError returns ERROR_MORE_DATA.
                //  This parameter receives the size of the buffer required, including the terminating null character.
                capacity = dwSize - 1uz;
                return S_OK;
            }

            capacity = 0;
            return HRESULT_FROM_WIN32(dwError);
        });
    }

    template <size_t Capacity>
    anemone_forceinline HRESULT GetUserName(string_buffer<wchar_t, Capacity>& result) noexcept
    {
        return AdaptStringBuffer(result, [](std::span<wchar_t> buffer, size_t& capacity) -> HRESULT
        {
            DWORD dwSize = static_cast<DWORD>(buffer.size() + 1uz);
            if (GetUserNameW(buffer.data(), &dwSize))
            {
                // MSDN:
                //  On output, the variable receives the number of TCHARs copied to the buffer, including the
                //  terminating null character.
                capacity = dwSize - 1uz;
                return S_OK;
            }

            DWORD dwError = GetLastError();
            if (dwError == ERROR_INSUFFICIENT_BUFFER)
            {
                // MSDN:
                //  If lpBuffer is too small, the function fails and GetLastError returns ERROR_INSUFFICIENT_BUFFER.
                //  This parameter receives the required buffer size, including the terminating null character.
                capacity = dwSize - 1uz;
                return S_OK;
            }

            capacity = 0;
            return HRESULT_FROM_WIN32(dwError);
        });
    }

    template <size_t Capacity>
    anemone_forceinline HRESULT GetUserPreferredUILanguages(string_buffer<wchar_t, Capacity>& result, DWORD flags, ULONG& numLanguages)
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
                return S_OK;
            }
        }

        return HRESULT_FROM_WIN32(GetLastError());
    }

    template <typename Callback = void(std::wstring_view)>
    anemone_forceinline HRESULT GetKnownFolderPath(KNOWNFOLDERID const& id, Callback&& callback) noexcept
    {
        SafeComTaskMemoryHandle<wchar_t> wPath{};

        HRESULT const hr = SHGetKnownFolderPath(id, 0, nullptr, wPath.ResetAndGetAddressOf());

        if (SUCCEEDED(hr))
        {
            if (wPath)
            {
                std::forward<Callback>(callback)(std::wstring_view{wPath.Get()});
            }
        }

        return hr;
    }

    template <size_t Capacity>
    anemone_forceinline HRESULT GetLogicalProcessorInformationEx(
        memory_buffer<Capacity>& buffer,
        LOGICAL_PROCESSOR_RELATIONSHIP relationshipType)
    {
        return AdaptMemoryBuffer(
            buffer,
            [&](std::span<std::byte> view, size_t& capacity)
        {
            DWORD dwSize = static_cast<DWORD>(buffer.size());
            if (::GetLogicalProcessorInformationEx(
                    relationshipType,
                    reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(view.data()),
                    &dwSize))
            {
                // MSDN:
                //  If the function succeeds, the return value is TRUE and at least one
                //  SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX structure is written to the output buffer.
                capacity = dwSize;
                return S_OK;
            }

            DWORD dwError = GetLastError();
            if (dwError == ERROR_INSUFFICIENT_BUFFER)
            {
                // MSDN:
                //  If the buffer is not large enough to contain all of the data, the function fails, GetLastError
                //  returns ERROR_INSUFFICIENT_BUFFER, and ReturnedLength is set to the buffer length required to
                //  contain all of the data.
                capacity = dwSize;
                return S_OK;
            }

            capacity = 0;
            return HRESULT_FROM_WIN32(dwError);
        });
    }

    template <typename Callback>
    anemone_forceinline void EnumerateLogicalProcessorInformation(std::span<std::byte const> buffer, Callback callback)
    {
        while (not buffer.empty())
        {
            SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX const& current = *reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX const*>(buffer.data());

            callback(current);

            buffer = buffer.subspan(current.Size);
        }
    }
}
