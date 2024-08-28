#pragma once
#include "AnemoneRuntime/Platform/Windows/Headers.hxx"
#include "AnemoneRuntime/Platform/Windows/Types.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Duration.hxx"
#include "AnemoneRuntime/DateTime.hxx"
#include "AnemoneRuntime/Geometry.hxx"

#include <span>
#include <string_view>
#include <cassert>
#include <memory>

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <shellapi.h>
#include <ShlObj.h>

ANEMONE_EXTERNAL_HEADERS_END

namespace Anemone::Platform
{
    constexpr ULONG_PTR IDI_MAIN_ICON = 2137u;

    inline constexpr UINT win32_GetKeyState_None = 0u;
    inline constexpr UINT win32_GetKeyState_Down = 0x8000u;
    inline constexpr UINT win32_GetKeyState_Locked = 0x0001u;

    anemone_forceinline void win32_RestoreUI() noexcept
    {
        while (ShowCursor(TRUE) < 0)
        {
            // do nothing
        }

        ReleaseCapture();

        ClipCursor(nullptr);
    }

    [[nodiscard]] anemone_forceinline bool win32_IsSystemVersion(
        uint32_t major,
        uint32_t minor) noexcept
    {
        OSVERSIONINFOEXW osie{
            .dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW),
            .dwMajorVersion = static_cast<DWORD>(major),
            .dwMinorVersion = static_cast<DWORD>(minor),
        };

        ULONGLONG conditionMask{};
        conditionMask = VerSetConditionMask(
            conditionMask,
            VER_MAJORVERSION,
            VER_GREATER_EQUAL);
        conditionMask = VerSetConditionMask(
            conditionMask,
            VER_MINORVERSION,
            VER_GREATER_EQUAL);

        return VerifyVersionInfoW(
                   &osie,
                   VER_MAJORVERSION | VER_MINORVERSION,
                   conditionMask) != FALSE;
    }

    template <typename CharT, size_t StaticCapacityT>
    class win32_string_buffer final
    {
    private:
        CharT* m_Data{};
        std::unique_ptr<CharT[]> m_Dynamic{};
        size_t m_Capacity{StaticCapacityT};
        size_t m_Size{};
        CharT m_Static[StaticCapacityT];

    public:
        constexpr win32_string_buffer() noexcept
            : m_Data{m_Static}
        {
        }

        constexpr explicit win32_string_buffer(std::basic_string_view<CharT> value)
        {
            this->resize_for_override(value.size() + 1);
            std::copy(value.begin(), value.end(), this->m_Data);
            this->trim(value.size());
        }

        win32_string_buffer(win32_string_buffer const&) = delete;
        win32_string_buffer(win32_string_buffer&&) = delete;
        win32_string_buffer& operator=(win32_string_buffer const&) = delete;
        win32_string_buffer& operator=(win32_string_buffer&&) = delete;

        ~win32_string_buffer() noexcept = default;

        constexpr void resize_for_override(size_t size)
        {
            if (size < StaticCapacityT)
            {
                // Reset back to static buffer.
                this->m_Dynamic = nullptr;
                this->m_Data = this->m_Static;
                this->m_Capacity = StaticCapacityT;
                this->m_Size = size;
            }
            else if (size > this->m_Capacity)
            {
                size_t const capacity = size;

                this->m_Dynamic = std::make_unique_for_overwrite<CharT[]>(capacity);
                this->m_Data = this->m_Dynamic.get();
                this->m_Capacity = capacity;
                this->m_Size = size;
            }
        }

        constexpr void trim(size_t size) noexcept
        {
            assert(size <= this->m_Capacity);
            this->m_Data[size] = {};
            this->m_Size = size;
        }

        [[nodiscard]] constexpr size_t capacity() const noexcept
        {
            return this->m_Capacity;
        }

        [[nodiscard]] constexpr operator const CharT*() const noexcept
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr CharT* data() noexcept
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr const CharT* data() const noexcept
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> as_view() const noexcept
        {
            return std::basic_string_view<CharT>{this->m_Data, this->m_Size};
        }

        [[nodiscard]] constexpr std::span<CharT> as_span() noexcept
        {
            return std::span<CharT>{this->m_Data, this->m_Size};
        }

        [[nodiscard]] constexpr std::span<CharT const> as_span() const noexcept
        {
            return std::span<CharT const>{this->m_Data, this->m_Size};
        }

        [[nodiscard]] constexpr std::span<CharT> as_buffer_span() noexcept
        {
            return std::span<CharT>{this->m_Data, this->m_Capacity};
        }

        [[nodiscard]] constexpr std::span<CharT const> as_buffer_span() const noexcept
        {
            return std::span<CharT const>{this->m_Data, this->m_Capacity};
        }
    };

    template <typename StringBufferT, typename CallbackT>
    bool win32_adapt_string_buffer(StringBufferT& buffer, CallbackT callback) noexcept
    {
        size_t requiredCapacity{};
        if (not callback(buffer.as_buffer_span(), requiredCapacity))
        {
            buffer.trim(0);
            return false;
        }

        if (requiredCapacity <= buffer.capacity())
        {
            assert(requiredCapacity != 0);
            buffer.trim(requiredCapacity - 1);
            return true;
        }

        size_t bufferLength{};
        do
        {
            bufferLength = requiredCapacity;

            buffer.resize_for_override(bufferLength);

            if (not callback(buffer.as_buffer_span(), requiredCapacity))
            {
                buffer.trim(0);
                return false;
            }
        } while (requiredCapacity > bufferLength);

        assert(requiredCapacity != 0);
        buffer.trim(requiredCapacity - 1);
        return true;
    }

    using win32_FilePathW = win32_string_buffer<wchar_t, MAX_PATH>;
    using win32_FilePathA = win32_string_buffer<char, MAX_PATH>;

    inline void win32_AddDirectorySeparator(std::wstring& path)
    {
        if (path.empty())
        {
            return;
        }

        wchar_t const last = path.back();

        if ((last != L'/') and (last != L'\\'))
        {
            path.push_back(L'/');
        }
    }
}

namespace Anemone::Platform
{
    template <size_t CapacityT>
    anemone_forceinline bool win32_QueryFullProcessImageName(win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        HANDLE const hProcess = GetCurrentProcess();
        return win32_adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
        {
            DWORD dwSize = static_cast<DWORD>(buffer.size());

            if (QueryFullProcessImageNameW(hProcess, 0, buffer.data(), &dwSize))
            {
                capacity = dwSize + 1;
                return true;
            }

            if (DWORD const dwError = GetLastError(); dwError == ERROR_INSUFFICIENT_BUFFER)
            {
                capacity = static_cast<size_t>(dwSize) * 2;
                return true;
            }

            capacity = 0;
            return false;
        });
    }

    anemone_forceinline bool win32_WidenString(std::wstring& result, std::string_view value) noexcept
    {
        int const length = static_cast<int>(value.size());

        if (length > 0)
        {
            int const required = MultiByteToWideChar(
                CP_UTF8,
                0,
                value.data(),
                length,
                nullptr,
                0);

            if (required == 0)
            {
                result.clear();
                return false;
            }

            result.resize_and_overwrite(static_cast<size_t>(required), [&](wchar_t* ptr, size_t size) -> size_t
            {
                int const processed = MultiByteToWideChar(
                    CP_UTF8,
                    0,
                    value.data(),
                    static_cast<int>(value.size()),
                    ptr,
                    static_cast<int>(size));

                assert(required == processed);
                return static_cast<size_t>(processed);
            });
            return true;
        }

        result.clear();
        return true;
    }

    template <size_t CapacityT>
    anemone_forceinline bool win32_WidenString(win32_string_buffer<wchar_t, CapacityT>& result, std::string_view value) noexcept
    {
        int const length = static_cast<int>(value.length());

        if (length > 0)
        {
            int const required = MultiByteToWideChar(
                CP_UTF8,
                0,
                value.data(),
                length,
                nullptr,
                0);

            if (required == 0)
            {
                result.trim(0);
                return false;
            }

            result.resize_for_override(static_cast<size_t>(required) + 1);

            std::span<wchar_t> const writable = result.as_buffer_span();

            int const processed = MultiByteToWideChar(
                CP_UTF8,
                0,
                value.data(),
                length,
                writable.data(),
                static_cast<int>(writable.size()));

            if (processed == required)
            {
                result.trim(static_cast<size_t>(processed));
                return true;
            }

            return false;
        }

        result.trim(0);
        return true;
    }

    anemone_forceinline bool win32_NarrowString(std::string& result, std::wstring_view value) noexcept
    {
        if (not value.empty())
        {
            int const required = WideCharToMultiByte(
                CP_UTF8,
                0,
                value.data(),
                static_cast<int>(value.size()),
                nullptr,
                0,
                nullptr,
                nullptr);

            if (required == 0)
            {
                result.clear();
                return false;
            }

            result.resize_and_overwrite(static_cast<size_t>(required), [&](char* ptr, size_t size) -> size_t
            {
                int const processed = WideCharToMultiByte(
                    CP_UTF8,
                    0,
                    value.data(),
                    static_cast<int>(value.size()),
                    ptr,
                    static_cast<int>(size),
                    nullptr,
                    nullptr);

                assert(required == processed);
                return static_cast<size_t>(processed);
            });
            return true;
        }

        result.clear();
        return true;
    }

    template <size_t CapacityT>
    anemone_forceinline bool win32_NarrowString(win32_string_buffer<char, CapacityT>& result, std::wstring_view value) noexcept
    {
        int const length = static_cast<int>(value.length());

        if (length > 0)
        {
            int const required = WideCharToMultiByte(
                CP_UTF8,
                0,
                value.data(),
                length,
                nullptr,
                0,
                nullptr,
                nullptr);

            if (required == 0)
            {
                result.trim(0);
                return false;
            }

            result.resize_for_override(static_cast<size_t>(required) + 1);

            std::span<char> const writable = result.as_buffer_span();

            int const processed = WideCharToMultiByte(
                CP_UTF8,
                0,
                value.data(),
                length,
                writable.data(),
                static_cast<int>(writable.size()),
                nullptr,
                nullptr);

            if (processed == required)
            {
                result.trim(static_cast<size_t>(processed));
                return true;
            }

            return false;
        }

        result.trim(0);
        return true;
    }

    template <size_t CapacityT>
    anemone_forceinline bool win32_GetCurrentDirectory(win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return win32_adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
        {
            if (DWORD const dwLength = GetCurrentDirectoryW(static_cast<DWORD>(buffer.size()), buffer.data()); dwLength != 0)
            {
                capacity = static_cast<size_t>(dwLength) + 1;
                return true;
            }

            capacity = 0;
            return false;
        });
    }

    template <typename CallbackT = void(std::wstring_view)>
    anemone_forceinline bool win32_GetKnownFolderPath(KNOWNFOLDERID const& id, CallbackT callback) noexcept
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

    template <size_t CapacityT>
    anemone_forceinline bool win32_GetEnvironmentVariable(win32_string_buffer<wchar_t, CapacityT>& result, const wchar_t* name) noexcept
    {
        return win32_adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
        {
            SetLastError(ERROR_SUCCESS);
            DWORD dwLength = GetEnvironmentVariableW(name, buffer.data(), static_cast<DWORD>(buffer.size()));

            if ((dwLength == 0) and GetLastError() != ERROR_SUCCESS)
            {
                capacity = 0;
                return false;
            }

            if (dwLength < buffer.size())
            {
                ++dwLength;
            }

            capacity = dwLength;
            return true;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool win32_QueryRegistry(win32_string_buffer<wchar_t, CapacityT>& result, HKEY key, const wchar_t* subkey, const wchar_t* name) noexcept
    {
        if ((key == nullptr) or (subkey == nullptr) or (name == nullptr))
        {
            return false;
        }

        bool succeeded = false;

        for (int32_t const flag : {KEY_WOW64_32KEY, KEY_WOW64_64KEY})
        {
            if (succeeded)
            {
                break;
            }

            HKEY current = nullptr;

            if (RegOpenKeyExW(key, subkey, 0, static_cast<REGSAM>(KEY_READ | flag), &current) == ERROR_SUCCESS)
            {
                if (win32_adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
                {
                    DWORD const dwLength = static_cast<DWORD>(buffer.size());

                    DWORD dwSize = dwLength * sizeof(wchar_t);
                    LSTATUS const status = RegQueryValueExW(
                        current,
                        name,
                        nullptr,
                        nullptr,
                        reinterpret_cast<LPBYTE>(buffer.data()),
                        &dwSize);

                    if ((status == ERROR_MORE_DATA) or (status == ERROR_SUCCESS))
                    {
                        DWORD const dwFinal = (dwSize / sizeof(wchar_t));
                        capacity = dwFinal + 1;
                        return true;
                    }

                    capacity = 0;
                    return false;
                }))
                {
                    succeeded = true;
                }

                RegCloseKey(current);
            }
        }

        return succeeded;
    }

    template <size_t CapacityT>
    anemone_forceinline bool win32_GetSystemDirectory(win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return win32_adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
        {
            UINT const length = GetSystemDirectoryW(buffer.data(), static_cast<UINT>(buffer.size()));

            if (length == 0)
            {
                capacity = 0;
                return false;
            }

            capacity = length + 1;
            return true;
        });
    }

    // TODO: Test this function
    template <size_t CapacityT>
    anemone_forceinline bool GetFullPathName(win32_string_buffer<wchar_t, CapacityT>& result, const wchar_t* path) noexcept
    {
        return win32_adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
        {
            DWORD const length = GetFullPathNameW(path, static_cast<DWORD>(buffer.size()), buffer.data(), nullptr);

            if (length == 0)
            {
                capacity = 0;
                return false;
            }

            capacity = length + 1;
            return true;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool win32_GetUserDefaultLocaleName(win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        std::span<wchar_t> writable = result.as_buffer_span();
        if (int const length = GetUserDefaultLocaleName(writable.data(), static_cast<int>(writable.size())); length > 0)
        {
            result.trim(static_cast<size_t>(length - 1));
            return true;
        }

        result.trim(0);
        return false;
    }

    template <size_t CapacityT>
    anemone_forceinline bool win32_GetComputerName(win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return win32_adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
        {
            DWORD dwSize = static_cast<DWORD>(buffer.size());
            if (GetComputerNameW(buffer.data(), &dwSize))
            {
                // Trimmed
                capacity = static_cast<size_t>(dwSize) + 1;
                return true;
            }

            if (GetLastError() == ERROR_BUFFER_OVERFLOW)
            {
                // Retry with that buffer size.
                capacity = dwSize;
                return true;
            }

            capacity = 0;
            return false;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool win32_GetComputerNameExW(COMPUTER_NAME_FORMAT format, win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return win32_adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
        {
            DWORD dwSize = static_cast<DWORD>(buffer.size());
            if (GetComputerNameExW(format, buffer.data(), &dwSize))
            {
                // Trimmed
                capacity = static_cast<size_t>(dwSize) + 1;
                return true;
            }

            if (GetLastError() == ERROR_MORE_DATA)
            {
                // Retry with that buffer size.
                capacity = dwSize;
                return true;
            }

            capacity = 0;
            return false;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool win32_GetUserName(win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return win32_adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
        {
            DWORD dwSize = static_cast<DWORD>(buffer.size());
            if (GetUserNameW(buffer.data(), &dwSize))
            {
                // Trimmed
                capacity = dwSize;
                return true;
            }

            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                // Retry with that buffer size.
                capacity = dwSize;
                return true;
            }

            capacity = 0;
            return false;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool win32_GetTempPath(win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return win32_adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
        {
            DWORD const length = GetTempPathW(static_cast<DWORD>(buffer.size()), buffer.data());

            if (length != 0)
            {
                capacity = static_cast<size_t>(length) + 1;
                return true;
            }

            capacity = 0;
            return false;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool win32_GetLongPathName(win32_string_buffer<wchar_t, CapacityT>& result, const wchar_t* path) noexcept
    {
        return win32_adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
        {
            DWORD const length = GetLongPathNameW(path, buffer.data(), static_cast<DWORD>(buffer.size()));

            if (length != 0)
            {
                capacity = length;
                return true;
            }

            capacity = 0;
            return false;
        });
    }

    template <size_t CapacityT>
    anemone_forceinline bool win32_GetUserPreferredUILanguages(win32_string_buffer<wchar_t, CapacityT>& result, DWORD flags, ULONG& numLanguages)
    {
        return win32_adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
        {
            ULONG uSize = static_cast<ULONG>(buffer.size());

            if (GetUserPreferredUILanguages(flags, &numLanguages, buffer.data(), &uSize))
            {
                // Trim to result.
                capacity = uSize;
                return true;
            }

            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                // Retry with 2x buffer size.
                capacity = buffer.size() * 2;
                return true;
            }

            capacity = 0;
            return false;
        });
    }

    // SHGetFolderPathW; todo
    // GetModuleFileNameW
}

namespace Anemone::Platform
{
    static constexpr int64_t win32_DateAdjustOffset = 504911231999999999;
    inline constexpr int64_t win32_TicksPerSecond = 1'000'000'000 / 100;

    constexpr Duration win32_into_Duration(FILETIME const value, int64_t offset) noexcept
    {
        int64_t const ticks = std::bit_cast<int64_t>(value) + offset;

        return Duration{
            .Seconds = (ticks / win32_TicksPerSecond),
            .Nanoseconds = (ticks % win32_TicksPerSecond) * 100,
        };
    }

    constexpr Duration win32_into_Duration(FILETIME const value) noexcept
    {
        int64_t const ticks = std::bit_cast<int64_t>(value);

        return Duration{
            .Seconds = (ticks / win32_TicksPerSecond),
            .Nanoseconds = (ticks % win32_TicksPerSecond) * 100,
        };
    }

    constexpr FILETIME win32_into_FILETIME(Duration const value) noexcept
    {
        int64_t const ticks = (value.Seconds * win32_TicksPerSecond) + (value.Nanoseconds / 100);

        return std::bit_cast<FILETIME>(ticks - win32_DateAdjustOffset);
    }

    constexpr DateTime win32_into_DateTime(FILETIME const& value)
    {
        return DateTime{
            .Inner = win32_into_Duration(value, win32_DateAdjustOffset)};
    }

    constexpr FILETIME win32_into_FILETIME(DateTime const& value)
    {
        return win32_into_FILETIME(value.Inner);
    }
}

namespace Anemone::Platform
{
    inline constexpr size_t win32_DefaultBufferSize = size_t{64} << 10u;

    constexpr OVERLAPPED win32_GetOverlappedForPosition(int64_t position)
    {
        LARGE_INTEGER const li = std::bit_cast<LARGE_INTEGER>(position);

        OVERLAPPED overlapped{};
        overlapped.Offset = li.LowPart;
        overlapped.OffsetHigh = li.HighPart;
        return overlapped;
    }
}

namespace Anemone::Platform
{
    inline DWORD win32_ValidateTimeoutDuration(Duration const& value) noexcept
    {
        int64_t const milliseconds = value.ToMilliseconds();

        if ((milliseconds < 0) || (milliseconds > std::numeric_limits<int32_t>::max()))
        {
            AE_BUGCHECK("Duration timeout out of range");
        }

        return static_cast<DWORD>(milliseconds);
    }

    inline DWORD win32_ValidateTimeoutDuration(int32_t milliseconds) noexcept
    {
        if (milliseconds < 0)
        {
            AE_BUGCHECK("Duration timeout out of range");
        }

        return static_cast<DWORD>(milliseconds);
    }
}

namespace Anemone::Platform
{
    inline bool win32_LocalSystemTimeToFileTime(const SYSTEMTIME& localTime, FILETIME& fileTime)
    {
        SYSTEMTIME systemTime;

        BOOL result = TzSpecificLocalTimeToSystemTime(nullptr, &localTime, &systemTime);

        if (result)
        {
            result = SystemTimeToFileTime(&systemTime, &fileTime);
        }

        return result;
    }

    inline bool win32_GetLocalTimeAsFileTime(FILETIME& fileTime)
    {
        SYSTEMTIME systemTime;
        GetLocalTime(&systemTime);

        return SystemTimeToFileTime(&systemTime, &fileTime);
    }
}

namespace Anemone::Platform
{
    constexpr Point<int32_t> win32_into_Point(POINT const& value) noexcept
    {
        return {
            .X = value.x,
            .Y = value.y,
        };
    }

    constexpr POINT win32_into_POINT(Point<int32_t> const& value) noexcept
    {
        return {
            .x = value.X,
            .y = value.Y,
        };
    }

    constexpr Size<int32_t> win32_into_Size(SIZE const& value) noexcept
    {
        return {
            .Width = value.cx,
            .Height = value.cy,
        };
    }

    constexpr Size<int32_t> win32_into_Size(RECT const& value) noexcept
    {
        return {
            .Width = value.right - value.left,
            .Height = value.bottom - value.top,
        };
    }

    constexpr SIZE win32_into_SIZE(Size<int32_t> const& value) noexcept
    {
        return {
            .cx = value.Width,
            .cy = value.Height,
        };
    }

    constexpr Rectangle<int32_t> win32_into_Rectangle(RECT const& value) noexcept
    {
        return {
            .X = value.left,
            .Y = value.top,
            .Width = value.right - value.left,
            .Height = value.bottom - value.top,
        };
    }

    constexpr RECT win32_into_RECT(Rectangle<int32_t> const& value) noexcept
    {
        return {
            .left = value.X,
            .top = value.Y,
            .right = value.X + value.Width,
            .bottom = value.Y + value.Height,
        };
    }
}
