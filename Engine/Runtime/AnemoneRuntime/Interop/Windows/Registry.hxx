#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"

namespace Anemone::Interop::Windows
{
    struct RegistryKey final
    {
    private:
        HKEY m_key{};

        static constexpr REGSAM GetMask(bool writable)
        {
            REGSAM accessMask = KEY_READ;

            if (writable)
            {
                accessMask |= KEY_WRITE;
            }

            return accessMask;
        }

    public:
        RegistryKey() = default;

        explicit RegistryKey(HKEY key)
            : m_key{key}
        {
        }

        RegistryKey(RegistryKey const&) = delete;

        RegistryKey(RegistryKey&& other) noexcept
            : m_key{std::exchange(other.m_key, nullptr)}
        {
        }

        RegistryKey& operator=(RegistryKey const&) = delete;

        RegistryKey& operator=(RegistryKey&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                this->Close();
                this->m_key = std::exchange(other.m_key, nullptr);
            }

            return *this;
        }

        ~RegistryKey()
        {
            this->Close();
        }

        static RegistryKey Open(HKEY key, const wchar_t* name, bool writable)
        {
            if (HKEY result{}; RegOpenKeyExW(key, name, 0, GetMask(writable), &result) == ERROR_SUCCESS)
            {
                return RegistryKey{result};
            }

            return {};
        }

        static RegistryKey Open(HKEY key, const wchar_t* name)
        {
            return Open(key, name, false);
        }

        RegistryKey OpenSubKey(const wchar_t* name, bool writable) const
        {
            if (this->m_key)
            {
                return Open(this->m_key, name, writable);
            }

            return {};
        }

        RegistryKey OpenSubKey(const wchar_t* name) const
        {
            return OpenSubKey(name, false);
        }

        void Close()
        {
            if (this->m_key != nullptr)
            {
                RegCloseKey(this->m_key);
                this->m_key = nullptr;
            }
        }

        [[nodiscard]] HKEY Get() const
        {
            return this->m_key;
        }

        [[nodiscard]] explicit operator bool() const
        {
            return this->m_key != nullptr;
        }

        template <size_t Capacity, typename Callback = void(std::wstring_view value)>
        [[nodiscard]] bool ReadStrings(const wchar_t* name, string_buffer<wchar_t, Capacity>& storage, Callback&& callback) const
        {
            if (adapt_string_buffer(storage, [&](std::span<wchar_t> buffer, size_t& capacity)
            {
                DWORD dwType = 0;
                DWORD dwSize = static_cast<DWORD>(buffer.size() + 1uz) * sizeof(wchar_t);

                // REG_SZ requires a zero terminator.
                LSTATUS const status = RegQueryValueExW(
                    this->m_key,
                    name,
                    nullptr,
                    &dwType,
                    reinterpret_cast<LPBYTE>(buffer.data()),
                    &dwSize);

                if ((dwType != REG_SZ) and (dwType != REG_MULTI_SZ))
                {
                    capacity = 0;
                    return false;
                }

                if ((status == ERROR_MORE_DATA) or (status == ERROR_SUCCESS))
                {
                    // MSDN:
                    //  If the buffer specified by lpData parameter is not large enough to hold the data, the function
                    //  returns ERROR_MORE_DATA and stores the required buffer size in the variable pointed to by
                    //  lpcbData.
                    DWORD const dwCharCount = (dwSize / sizeof(wchar_t)) - 1u;
                    capacity = dwCharCount;
                    return true;
                }

                capacity = 0;
                return false;
            }))
            {
                // Storage buffer contains one or more string(s)
                auto view = storage.as_view();

                // Start from string beginning.
                std::wstring_view::size_type start{};

                while (true)
                {
                    // Find separator position.
                    std::wstring_view::size_type const pos = view.find_first_of(L'\0', start);

                    // Get part.
                    if (std::wstring_view const part = view.substr(start, pos - start); !part.empty())
                    {
                        std::forward<Callback>(callback)(part);
                    }

                    if (pos == std::wstring_view::npos)
                    {
                        break;
                    }

                    // Move past separator.
                    start = pos + 1;
                }

                return true;
            }

            return false;
        }

        template <size_t Capacity>
        [[nodiscard]] bool ReadString(const wchar_t* name, string_buffer<wchar_t, Capacity>& result) const
        {
            return adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
            {
                DWORD dwType = 0;
                DWORD dwSize = static_cast<DWORD>(buffer.size() + 1uz) * sizeof(wchar_t);

                // REG_SZ requires a zero terminator.
                LSTATUS const status = RegQueryValueExW(
                    this->m_key,
                    name,
                    nullptr,
                    &dwType,
                    reinterpret_cast<LPBYTE>(buffer.data()),
                    &dwSize);

                if ((dwType != REG_SZ) and (dwType != REG_MULTI_SZ))
                {
                    // Invalid type
                    capacity = 0;
                    return false;
                }

                if ((status == ERROR_MORE_DATA) or (status == ERROR_SUCCESS))
                {
                    // MSDN:
                    //  If the buffer specified by lpData parameter is not large enough to hold the data, the function
                    //  returns ERROR_MORE_DATA and stores the required buffer size in the variable pointed to by
                    //  lpcbData.
                    DWORD dwCharCount = (dwSize / sizeof(wchar_t)) - 1u;

                    if (dwType == REG_MULTI_SZ)
                    {
                        auto chars = buffer.subspan(0, dwCharCount);

                        for (wchar_t& ch : chars)
                        {
                            if (ch == L'\0')
                            {
                                ch = L'\n';
                            }
                        }
                    }

                    capacity = dwCharCount;
                    return true;
                }

                capacity = 0;
                return false;
            });
        }

        template <size_t Capacity>
        [[nodiscard]] bool ReadString(const char* name, string_buffer<char, Capacity>& result) const
        {
            return adapt_string_buffer(result, [&](std::span<char> buffer, size_t& capacity)
            {
                DWORD dwType = 0;
                DWORD dwSize = static_cast<DWORD>(buffer.size() + 1uz) * sizeof(char);

                // REG_SZ requires a zero terminator.
                LSTATUS const status = RegQueryValueExA(
                    this->m_key,
                    name,
                    nullptr,
                    &dwType,
                    reinterpret_cast<LPBYTE>(buffer.data()),
                    &dwSize);

                if ((dwType != REG_SZ) and (dwType != REG_MULTI_SZ))
                {
                    // Invalid type
                    capacity = 0;
                    return false;
                }

                if ((status == ERROR_MORE_DATA) or (status == ERROR_SUCCESS))
                {
                    // MSDN:
                    //  If the buffer specified by lpData parameter is not large enough to hold the data, the function
                    //  returns ERROR_MORE_DATA and stores the required buffer size in the variable pointed to by
                    //  lpcbData.
                    DWORD dwCharCount = (dwSize / sizeof(char)) - 1u;

                    if (dwType == REG_MULTI_SZ)
                    {
                        auto chars = buffer.subspan(0, dwCharCount);

                        for (char& ch : chars)
                        {
                            if (ch == '\0')
                            {
                                ch = '\n';
                            }
                        }
                    }

                    capacity = dwCharCount;
                    return true;
                }

                capacity = 0;
                return false;
            });
        }

        [[nodiscard]] bool Read(const wchar_t* name, DWORD& result) const
        {
            DWORD dwSize = sizeof(DWORD);
            DWORD dwType = 0;

            LSTATUS const status = RegQueryValueExW(
                this->m_key,
                name,
                nullptr,
                &dwType,
                reinterpret_cast<LPBYTE>(&result),
                &dwSize);

            return (dwType == REG_DWORD) and (status == ERROR_SUCCESS);
        }

        [[nodiscard]] bool Read(const wchar_t* name, DWORD64& result) const
        {
            DWORD dwSize = sizeof(DWORD64);
            DWORD dwType = 0;

            LSTATUS const status = RegQueryValueExW(
                this->m_key,
                name,
                nullptr,
                &dwType,
                reinterpret_cast<LPBYTE>(&result),
                &dwSize);

            return (dwType == REG_QWORD) and (status == ERROR_SUCCESS);
        }

        [[nodiscard]] bool DeleteValue(const wchar_t* name) const
        {
            return RegDeleteValueW(this->m_key, name) == ERROR_SUCCESS;
        }
    };

}
