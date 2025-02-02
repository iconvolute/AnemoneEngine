#pragma once
#include "AnemoneRuntime/Platform/Base/BaseInterop.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Duration.hxx"
#include "AnemoneRuntime/DateTime.hxx"
#include "AnemoneRuntime/Math/Point.hxx"
#include "AnemoneRuntime/Math/Size.hxx"
#include "AnemoneRuntime/Math/Rect.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <shellapi.h>
#include <ShlObj.h>

ANEMONE_EXTERNAL_HEADERS_END

namespace Anemone::Interop
{
    struct win32_SafeHandle final
    {
        HANDLE Handle = nullptr;

        [[nodiscard]] constexpr bool IsValid() const noexcept
        {
            return (this->Handle != nullptr) and (this->Handle != INVALID_HANDLE_VALUE);
        }

        win32_SafeHandle() noexcept = default;

        explicit win32_SafeHandle(HANDLE handle) noexcept
            : Handle{handle}
        {
        }

        win32_SafeHandle(win32_SafeHandle const&) = delete;

        win32_SafeHandle(win32_SafeHandle&& other) noexcept
            : Handle{std::exchange(other.Handle, nullptr)}
        {
        }

        win32_SafeHandle& operator=(win32_SafeHandle const&) = delete;

        win32_SafeHandle& operator=(win32_SafeHandle&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                this->Attach(other.Detach());
            }

            return *this;
        }

        ~win32_SafeHandle() noexcept
        {
            if (this->IsValid())
            {
                CloseHandle(this->Handle);
            }
        }

        HANDLE Detach()
        {
            return std::exchange(this->Handle, nullptr);
        }

        void Attach(HANDLE handle)
        {
            if (this->IsValid())
            {
                CloseHandle(this->Handle);
            }

            this->Handle = handle;
        }
    };

    constexpr bool win32_IsPathInvalidError(DWORD error)
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
}

namespace Anemone::Interop
{
    anemone_forceinline HCURSOR win32_LoadSystemCursor(LPCWSTR id)
    {
        return static_cast<HCURSOR>(LoadImageW(nullptr, id, IMAGE_CURSOR, 0, 0, LR_SHARED));
    }

    inline constexpr UINT win32_GetKeyState_None = 0u;
    inline constexpr UINT win32_GetKeyState_Down = 0x8000u;
    inline constexpr UINT win32_GetKeyState_Locked = 0x0001u;

    constexpr uint32_t win32_DecodeSurrogatePair(uint16_t high, uint16_t low)
    {
        uint32_t code = 0x10000u;
        code += (high & 0x03FFu) << 10u;
        code += (low & 0x03FFu);
        return code;
    }

    anemone_forceinline void win32_RestoreUI() noexcept
    {
        while (ShowCursor(TRUE) < 0)
        {
            // do nothing
        }

        ReleaseCapture();

        ClipCursor(nullptr);
    }

    anemone_forceinline bool win32_IsForeground()
    {
        DWORD dwProcessId{};
        GetWindowThreadProcessId(GetForegroundWindow(), &dwProcessId);
        return dwProcessId == GetCurrentProcessId();
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

    using win32_FilePathW = string_buffer<wchar_t, MAX_PATH>;
    using win32_FilePathA = string_buffer<char, MAX_PATH>;
}

namespace Anemone::Interop
{
    template <size_t CapacityT>
    anemone_forceinline bool win32_QueryFullProcessImageName(string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        HANDLE const hProcess = GetCurrentProcess();
        return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
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
    anemone_forceinline bool win32_WidenString(string_buffer<wchar_t, CapacityT>& result, std::string_view value) noexcept
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
    anemone_forceinline bool win32_NarrowString(string_buffer<char, CapacityT>& result, std::wstring_view value) noexcept
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
    anemone_forceinline bool win32_GetCurrentDirectory(string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
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
    anemone_forceinline bool win32_GetEnvironmentVariable(string_buffer<wchar_t, CapacityT>& result, const wchar_t* name) noexcept
    {
        return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
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
    anemone_forceinline bool win32_QueryRegistry(string_buffer<wchar_t, CapacityT>& result, HKEY key, const wchar_t* subkey, const wchar_t* name) noexcept
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
                if (adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
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

    anemone_forceinline bool win32_QueryRegistry(DWORD& result, HKEY key, const wchar_t* subkey, const wchar_t* name)
    {
        if ((key == nullptr) or (subkey == nullptr) or (name == nullptr))
        {
            return false;
        }

        for (int32_t const flag : {KEY_WOW64_32KEY, KEY_WOW64_64KEY})
        {
            HKEY current = nullptr;

            if (RegOpenKeyExW(key, subkey, 0, static_cast<REGSAM>(KEY_READ | flag), &current) == ERROR_SUCCESS)
            {
                DWORD dwSize = sizeof(DWORD);
                LSTATUS const status = RegQueryValueExW(
                    current,
                    name,
                    nullptr,
                    nullptr,
                    reinterpret_cast<LPBYTE>(&result),
                    &dwSize);

                if (status == ERROR_SUCCESS)
                {
                    return true;
                }
            }
        }

        return false;
    }

    struct win32_registry_key final
    {
    private:
        HKEY m_key{};

    public:
        win32_registry_key() = default;

        explicit win32_registry_key(HKEY key, const wchar_t* subKey, REGSAM access = KEY_READ | KEY_WRITE)
        {
            if (RegOpenKeyExW(key, subKey, 0, access, &this->m_key) != ERROR_SUCCESS)
            {
                this->m_key = nullptr;
            }
        }

        win32_registry_key(HKEY key)
            : m_key{key}
        {
        }

        win32_registry_key(win32_registry_key const&) = delete;

        win32_registry_key(win32_registry_key&& other) noexcept
            : m_key{std::exchange(other.m_key, nullptr)}
        {
        }

        win32_registry_key& operator=(win32_registry_key const&) = delete;

        win32_registry_key& operator=(win32_registry_key&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                this->close();
                this->m_key = std::exchange(other.m_key, nullptr);
            }

            return *this;
        }

        ~win32_registry_key() noexcept
        {
            this->close();
        }

        void close() noexcept
        {
            if (this->m_key)
            {
                RegCloseKey(this->m_key);
                this->m_key = nullptr;
            }
        }

        [[nodiscard]] HKEY get() const noexcept
        {
            return this->m_key;
        }

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return this->m_key != nullptr;
        }

        template <size_t CapacityT>
        bool read_string(const wchar_t* name, string_buffer<wchar_t, CapacityT>& result)
        {
            return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
            {
                DWORD dwType = 0;
                DWORD dwSize = static_cast<DWORD>(buffer.size());
                LSTATUS const status = RegQueryValueExW(
                    this->m_key,
                    name,
                    nullptr,
                    &dwType,
                    reinterpret_cast<LPBYTE>(buffer.data()),
                    &dwSize);

                if (dwType != REG_SZ)
                {
                    // Invalid type
                    capacity = 0;
                    return false;
                }

                if ((status == ERROR_MORE_DATA) or (status == ERROR_SUCCESS))
                {
                    DWORD const dwFinal = (dwSize / sizeof(wchar_t));
                    capacity = dwFinal;
                    return true;
                }

                capacity = 0;
                return false;
            });
        }

        bool delete_value(const wchar_t* name)
        {
            return RegDeleteValueW(this->m_key, name) == ERROR_SUCCESS;
        }
    };

    template <size_t CapacityT>
    anemone_forceinline bool win32_GetSystemDirectory(string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
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
    anemone_forceinline bool GetFullPathName(string_buffer<wchar_t, CapacityT>& result, const wchar_t* path) noexcept
    {
        return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
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
    anemone_forceinline bool win32_GetUserDefaultLocaleName(string_buffer<wchar_t, CapacityT>& result) noexcept
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
    anemone_forceinline bool win32_GetComputerName(string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
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
    anemone_forceinline bool win32_GetComputerNameExW(COMPUTER_NAME_FORMAT format, string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
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
    anemone_forceinline bool win32_GetUserName(string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
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
    anemone_forceinline bool win32_GetTempPath(string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        using Fn = decltype(&::GetTempPathW);

        HMODULE const hKernel32 = GetModuleHandleW(L"kernel32.dll");

        Fn getTempPath = reinterpret_cast<Fn>(reinterpret_cast<void*>(GetProcAddress(hKernel32, "GetTempPath2W"))); // NOLINT(bugprone-casting-through-void)

        if (not getTempPath)
        {
            getTempPath = &::GetTempPathW;
        }

        return adapt_string_buffer(result, [getTempPath](std::span<wchar_t> buffer, size_t& capacity)
        {
            DWORD const length = getTempPath(static_cast<DWORD>(buffer.size()), buffer.data());

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
    anemone_forceinline bool win32_GetLongPathName(string_buffer<wchar_t, CapacityT>& result, const wchar_t* path) noexcept
    {
        return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
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
    anemone_forceinline bool win32_GetUserPreferredUILanguages(string_buffer<wchar_t, CapacityT>& result, DWORD flags, ULONG& numLanguages)
    {
        return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
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

namespace Anemone::Interop
{
    anemone_forceinline bool win32_GetThreadContext(DWORD threadId, CONTEXT& context)
    {
        context = {};
        context.ContextFlags = CONTEXT_ALL;

        bool result = false;

        if (threadId == GetCurrentThreadId())
        {
            RtlCaptureContext(&context);
            result = true;
        }
        else
        {
            HANDLE const hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME, FALSE, threadId);

            if (hThread != nullptr)
            {
                DWORD const dwSuspended = SuspendThread(hThread);

                if (dwSuspended != static_cast<DWORD>(-1))
                {
                    GetThreadContext(hThread, &context);
                    ResumeThread(hThread);
                    result = true;
                }

                CloseHandle(hThread);
            }
        }

        return result;
    }
}

namespace Anemone::Interop
{
    constexpr wchar_t win32_DirectorySeparator = L'\\';

    constexpr bool win32_IsDirectorySeparator(wchar_t c)
    {
        return (c == L'/') or (c == L'\\');
    }

    constexpr std::wstring_view::size_type win32_FindLastDirectorySeparator(std::wstring_view path)
    {
        return path.find_last_of(LR"(/\)");
    }

    anemone_forceinline void win32_PathAddDirectorySeparator(std::wstring& path)
    {
        if (path.empty() or not win32_IsDirectorySeparator(path.back()))
        {
            path.push_back(win32_DirectorySeparator);
        }
    }

    anemone_forceinline void win32_PathRemoveDirectorySeparator(std::wstring& path)
    {
        if (not path.empty() and win32_IsDirectorySeparator(path.back()))
        {
            path.pop_back();
        }
    }

    anemone_forceinline void win32_PathPushFragment(std::wstring& path, std::wstring_view part)
    {
        if (path.empty())
        {
            path = part;
        }
        else
        {
            win32_PathAddDirectorySeparator(path);
            path.append(part);
        }
    }

    anemone_forceinline bool win32_PathPopFragment(std::wstring& path)
    {
        if (not path.empty())
        {
            // Find directory separator and remove everything after it
            auto const separator = win32_FindLastDirectorySeparator(path);

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

    anemone_forceinline bool win32_PathPopFragment(std::wstring& path, std::wstring& tail)
    {
        if (not path.empty())
        {
            // Find directory separator and remove everything after it
            auto const separator = win32_FindLastDirectorySeparator(path);

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

}

namespace Anemone::Interop
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

namespace Anemone::Interop
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

    constexpr void win32_UpdateOverlappedPosition(OVERLAPPED& overlapped, int64_t position)
    {
        LARGE_INTEGER const li = std::bit_cast<LARGE_INTEGER>(position);
        overlapped.Offset = li.LowPart;
        overlapped.OffsetHigh = li.HighPart;
    }

    constexpr DWORD win32_ValidateIoRequestLength(size_t value)
    {
        return static_cast<DWORD>(std::min(value, size_t{UINT32_MAX}));
    }
}

namespace Anemone::Interop
{
    inline DWORD win32_ValidateTimeoutDuration(Duration const& value) noexcept
    {
        int64_t const milliseconds = value.ToMilliseconds();

        if ((milliseconds < 0) || (milliseconds > std::numeric_limits<int32_t>::max()))
        {
            AE_PANIC("Duration timeout out of range");
        }

        return static_cast<DWORD>(milliseconds);
    }

    inline DWORD win32_ValidateTimeoutDuration(int32_t milliseconds) noexcept
    {
        if (milliseconds < 0)
        {
            AE_PANIC("Duration timeout out of range");
        }

        return static_cast<DWORD>(milliseconds);
    }
}

namespace Anemone::Interop
{
    anemone_forceinline void win32_FutexWait(std::atomic<int>& futex, int expected)
    {
        // Handle spurious wakes by checking the value after the wait.

        while (true)
        {
            WaitOnAddress(&futex, &expected, sizeof(expected), INFINITE);

            if (futex.load() != expected)
            {
                break;
            }
        }
    }

    template <typename PredicateT>
    anemone_forceinline void win32_FutexWaitUntil(std::atomic<int>& futex, PredicateT&& predicate)
    {
        int value = futex.load();

        while (not std::forward<PredicateT>(predicate)(value))
        {
            win32_FutexWait(futex, value);
            value = futex.load();
        }
    }

    anemone_forceinline bool win32_FutexWaitTimeout(std::atomic<int>& futex, int expected, Duration const& timeout)
    {
        DWORD dwTimeout = win32_ValidateTimeoutDuration(timeout);

        UINT64 start;
        QueryUnbiasedInterruptTime(&start);

        UINT64 elapsed = 0;

        while (true)
        {
            if (elapsed >= dwTimeout)
            {
                return false;
            }

            DWORD const dwPartialTimeout = static_cast<DWORD>(dwTimeout - elapsed);

            BOOL const rc = WaitOnAddress(&futex, &expected, sizeof(expected), dwPartialTimeout);

            if (rc == FALSE)
            {
                DWORD const dwError = GetLastError();

                if (dwError == ERROR_TIMEOUT)
                {
                    return false;
                }
                else
                {
                    AE_PANIC("Futex wait failed: {}", dwError);
                }
            }
            else
            {
                if (futex.load() != expected)
                {
                    return true;
                }
            }

            UINT64 current;
            QueryUnbiasedInterruptTime(&current);

            elapsed = (current - start) / 10'000;
        }
    }

    anemone_forceinline bool win32_FutexWait(std::atomic<int>& futex, int expected, Duration const& timeout)
    {
        // Handle spurious wakes by checking the value after the wait.

        DWORD const dwTimeout = win32_ValidateTimeoutDuration(timeout);

        while (true)
        {
            BOOL const rc = WaitOnAddress(&futex, &expected, sizeof(expected), dwTimeout);

            if (rc == FALSE)
            {
                DWORD const dwError = GetLastError();

                if (dwError == ERROR_TIMEOUT)
                {
                    // Wait timed out
                    return false;
                }
                else
                {
                    AE_PANIC("Futex wait failed: {}", dwError);
                }
            }
            else
            {
                if (futex.load() != expected)
                {
                    // Value changed before timeout.
                    return true;
                }
            }
        }
    }
}

namespace Anemone::Interop
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

namespace Anemone::Interop
{
    constexpr Math::PointF win32_into_Point(POINT const& value) noexcept
    {
        return {
            .X = static_cast<float>(value.x),
            .Y = static_cast<float>(value.y),
        };
    }

    constexpr Math::PointF win32_into_Point(RECT const& value) noexcept
    {
        return {
            .X = static_cast<float>(value.left),
            .Y = static_cast<float>(value.top),
        };
    }

    constexpr POINT win32_into_POINT(Math::PointF const& value) noexcept
    {
        return {
            .x = static_cast<LONG>(value.X),
            .y = static_cast<LONG>(value.Y),
        };
    }

    constexpr Math::SizeF win32_into_Size(SIZE const& value) noexcept
    {
        return {
            .Width = static_cast<float>(value.cx),
            .Height = static_cast<float>(value.cy),
        };
    }

    constexpr Math::SizeF win32_into_Size(RECT const& value) noexcept
    {
        return {
            .Width = static_cast<float>(value.right - value.left),
            .Height = static_cast<float>(value.bottom - value.top),
        };
    }

    constexpr SIZE win32_into_SIZE(Math::SizeF const& value) noexcept
    {
        return {
            .cx = static_cast<LONG>(value.Width),
            .cy = static_cast<LONG>(value.Height),
        };
    }

    constexpr Math::RectF win32_into_Rectangle(RECT const& value) noexcept
    {
        return {
            .X = static_cast<float>(value.left),
            .Y = static_cast<float>(value.top),
            .Width = static_cast<float>(value.right - value.left),
            .Height = static_cast<float>(value.bottom - value.top),
        };
    }

    constexpr RECT win32_into_RECT(Math::RectF const& value) noexcept
    {
        return {
            .left = static_cast<LONG>(value.X),
            .top = static_cast<LONG>(value.Y),
            .right = static_cast<LONG>(value.X + value.Width),
            .bottom = static_cast<LONG>(value.Y + value.Height),
        };
    }
}

namespace Anemone::Interop
{
    class win32_ClipboardSession final
    {
    private:
        bool m_opened{};

    public:
        win32_ClipboardSession() noexcept
        {
            if (OpenClipboard(nullptr))
            {
                this->m_opened = true;
            }
        }

        win32_ClipboardSession(win32_ClipboardSession const&) = delete;
        win32_ClipboardSession(win32_ClipboardSession&&) = delete;
        win32_ClipboardSession& operator=(win32_ClipboardSession const&) = delete;
        win32_ClipboardSession& operator=(win32_ClipboardSession&&) = delete;

        ~win32_ClipboardSession() noexcept
        {
            if (this->m_opened)
            {
                CloseClipboard();
            }
        }

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return this->m_opened;
        }
    };

    class win32_HGlobalMemoryLock final
    {
    private:
        HGLOBAL m_handle{};
        void* m_data{};

    public:
        win32_HGlobalMemoryLock(HGLOBAL handle) noexcept
            : m_handle{handle}
        {
            if (handle != nullptr)
            {
                this->m_data = GlobalLock(handle);
            }
        }
        win32_HGlobalMemoryLock(win32_HGlobalMemoryLock const&) = delete;
        win32_HGlobalMemoryLock(win32_HGlobalMemoryLock&&) = delete;
        win32_HGlobalMemoryLock& operator=(win32_HGlobalMemoryLock const&) = delete;
        win32_HGlobalMemoryLock& operator=(win32_HGlobalMemoryLock&&) = delete;
        ~win32_HGlobalMemoryLock() noexcept
        {
            if (this->m_data != nullptr)
            {
                GlobalUnlock(this->m_handle);
            }
        }
        [[nodiscard]] explicit operator bool() const noexcept
        {
            return this->m_data != nullptr;
        }
        [[nodiscard]] void* data() noexcept
        {
            return this->m_data;
        }
        [[nodiscard]] void const* data() const noexcept
        {
            return this->m_data;
        }
    };
}

namespace Anemone::Interop
{
    anemone_forceinline PIMAGE_NT_HEADERS win32_GetImageNtHeaders(HMODULE h)
    {
        return std::bit_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<UINT_PTR>(h) + reinterpret_cast<PIMAGE_DOS_HEADER>(h)->e_lfanew);
    }

    anemone_forceinline bool win32_IsConsoleApplication(HMODULE h)
    {
        PIMAGE_NT_HEADERS const ntHeaders = win32_GetImageNtHeaders(h);
        return (ntHeaders->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI);
    }

    anemone_forceinline bool win32_IsConsoleHandle(HANDLE h)
    {
        DWORD dwMode = 0;
        return GetConsoleMode(h, &dwMode) != FALSE;
    }

    // HARD REQUIREMENT: Buffer must be zero terminated!
    anemone_forceinline void win32_OutputDebugString(const char* s, size_t n)
    {
        ULONG_PTR const args[2] = {n, reinterpret_cast<ULONG_PTR>(s)};
        RaiseException(DBG_PRINTEXCEPTION_C, 0, 2, args);
    }
}
