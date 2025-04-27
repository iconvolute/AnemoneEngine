#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"

namespace Anemone::Interop::Windows
{
    constexpr bool IsPathInvalidError(DWORD error)
    {
        switch (error)
        {
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
        case ERROR_NOT_READY:
        case ERROR_INVALID_NAME:
        case ERROR_BAD_PATHNAME:
        case ERROR_BAD_NETPATH:
        case ERROR_BAD_NET_NAME:
        case ERROR_INVALID_PARAMETER:
        case ERROR_NETWORK_UNREACHABLE:
        case ERROR_NETWORK_ACCESS_DENIED:
        case ERROR_INVALID_HANDLE:
        case ERROR_FILENAME_EXCED_RANGE:
            return true;

        default:
            return false;
        }
    }
    
    using FilePathW = string_buffer<wchar_t, MAX_PATH>;
    using FilePathA = string_buffer<char, MAX_PATH>;
    
    inline constexpr size_t DefaultBufferSize = size_t{64} << 10u;

    constexpr OVERLAPPED GetOverlappedForPosition(int64_t position)
    {
        LARGE_INTEGER const li = std::bit_cast<LARGE_INTEGER>(position);

        OVERLAPPED overlapped{};
        overlapped.Offset = li.LowPart;
        overlapped.OffsetHigh = li.HighPart;
        return overlapped;
    }

    constexpr void UpdateOverlappedPosition(OVERLAPPED& overlapped, int64_t position)
    {
        LARGE_INTEGER const li = std::bit_cast<LARGE_INTEGER>(position);
        overlapped.Offset = li.LowPart;
        overlapped.OffsetHigh = li.HighPart;
    }

    constexpr DWORD ValidateIoRequestLength(size_t value)
    {
        return static_cast<DWORD>(std::min(value, size_t{UINT32_MAX}));
    }

    constexpr wchar_t DirectorySeparator = L'\\';

    constexpr bool IsDirectorySeparator(wchar_t c)
    {
        return (c == L'/') or (c == L'\\');
    }

    constexpr std::wstring_view::size_type FindLastDirectorySeparator(std::wstring_view path)
    {
        return path.find_last_of(LR"(/\)");
    }

    anemone_forceinline void PathAddDirectorySeparator(std::wstring& path)
    {
        if (path.empty() or not IsDirectorySeparator(path.back()))
        {
            path.push_back(DirectorySeparator);
        }
    }

    anemone_forceinline void PathRemoveDirectorySeparator(std::wstring& path)
    {
        if (not path.empty() and IsDirectorySeparator(path.back()))
        {
            path.pop_back();
        }
    }

    anemone_forceinline void PathPushFragment(std::wstring& path, std::wstring_view part)
    {
        if (path.empty())
        {
            path = part;
        }
        else
        {
            PathAddDirectorySeparator(path);
            path.append(part);
        }
    }

    anemone_forceinline bool PathPopFragment(std::wstring& path)
    {
        if (not path.empty())
        {
            // Find directory separator and remove everything after it
            auto const separator = FindLastDirectorySeparator(path);

            if (separator != std::string_view::npos)
            {
                path.resize(separator);
            }
            else
            {
                path.clear();
            }

            return true;
        }

        return false;
    }

    anemone_forceinline bool PathPopFragment(std::wstring& path, std::wstring& tail)
    {
        if (not path.empty())
        {
            // Find directory separator and remove everything after it
            auto const separator = FindLastDirectorySeparator(path);

            if (separator != std::string_view::npos)
            {
                // Tail is everything after the separator
                tail = path.substr(separator + 1);
                path.resize(separator);
            }
            else
            {
                // Path don't contain any directory separators, so the whole path is the tail
                tail = std::move(path);
                path.clear();
            }

            return true;
        }

        tail.clear();
        return false;
    }
    
    // TODO: Test this function
    template <size_t CapacityT>
    anemone_forceinline bool GetFullPathName(string_buffer<wchar_t, CapacityT>& result, const wchar_t* path) noexcept
    {
        return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
        {
            // MSDN:
            //  The size of the buffer to receive the null-terminated string for the drive and path, in TCHARs.
            DWORD length = GetFullPathNameW(path, static_cast<DWORD>(buffer.size() + 1uz), buffer.data(), nullptr);

            if (length == 0)
            {
                capacity = 0;
                return false;
            }

            // MSDN:
            //  If the function succeeds, the return value is the length, in TCHARs, of the string copied to lpBuffer,
            //  not including the terminating null character.

            if (length > buffer.size())
            {
                // MSDN:
                //  If the lpBuffer buffer is too small to contain the path, the return value is the size, in TCHARs,
                //  of the buffer that is required to hold the path and the terminating null character.
                --length;
            }

            capacity = length;
            return true;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool GetTempPath(string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        using Fn = decltype(&::GetTempPathW);

        HMODULE const hKernel32 = GetModuleHandleW(L"kernel32.dll");

        Fn getTempPath = std::bit_cast<Fn>(GetProcAddress(hKernel32, "GetTempPathW"));

        if (not getTempPath)
        {
            getTempPath = &::GetTempPathW;
        }

        return adapt_string_buffer(result, [getTempPath](std::span<wchar_t> buffer, size_t& capacity)
        {
            DWORD length = getTempPath(static_cast<DWORD>(buffer.size() + 1uz), buffer.data());

            if (length != 0)
            {
                // MSDN:
                //  If the function succeeds, the return value is the length, in TCHARs, of the string copied to
                //  lpBuffer, not including the terminating null character.
                //


                if (length > buffer.size())
                {
                    // MSDN:
                    //  If the return value is greater than nBufferLength, the return value is the length, in TCHARs,
                    //  of the buffer required to hold the path.
                    --length;
                }

                capacity = static_cast<size_t>(length);
                return true;
            }

            capacity = 0;
            return false;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool GetLongPathName(string_buffer<wchar_t, CapacityT>& result, const wchar_t* path) noexcept
    {
        return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
        {
            DWORD length = GetLongPathNameW(path, buffer.data(), static_cast<DWORD>(buffer.size() + 1uz));

            if (length != 0)
            {
                // MSDN:
                //  If the function succeeds, the return value is the length, in TCHARs, of the string copied to
                //  lpszLongPath, not including the terminating null character.

                if (length > buffer.size())
                {
                    // MSDN:
                    //  If the lpBuffer buffer is too small to contain the path, the return value is the size,
                    //  in TCHARs, of the buffer that is required to hold the path and the terminating null character.
                    --length;
                }

                capacity = length;
                return true;
            }

            capacity = 0;
            return false;
        });
    }

}
