#pragma once
#include "AnemoneInterop/Windows/Headers.hxx"
#include "AnemoneInterop/SafeHandle.hxx"

namespace Anemone::Interop::Windows
{
    [[nodiscard]] anemone_forceinline bool IsSystemVersion(
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

    class ClipboardSession final
    {
    private:
        bool m_opened{};

    public:
        explicit ClipboardSession(HWND hWndNewOwner) noexcept
        {
            if (OpenClipboard(hWndNewOwner))
            {
                this->m_opened = true;
            }
        }

        ClipboardSession(ClipboardSession const&) = delete;
        ClipboardSession(ClipboardSession&&) = delete;
        ClipboardSession& operator=(ClipboardSession const&) = delete;
        ClipboardSession& operator=(ClipboardSession&&) = delete;

        ~ClipboardSession() noexcept
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

    template <typename T>
    class HGlobalMemoryLock final
    {
    private:
        HGLOBAL m_handle{};
        T* m_data{};

    public:
        HGlobalMemoryLock(HGLOBAL handle) noexcept
            : m_handle{handle}
        {
            if (handle != nullptr)
            {
                this->m_data = GlobalLock(handle);
            }
        }
        HGlobalMemoryLock(HGlobalMemoryLock const&) = delete;
        HGlobalMemoryLock(HGlobalMemoryLock&&) = delete;
        HGlobalMemoryLock& operator=(HGlobalMemoryLock const&) = delete;
        HGlobalMemoryLock& operator=(HGlobalMemoryLock&&) = delete;
        ~HGlobalMemoryLock() noexcept
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
        [[nodiscard]] T* data() noexcept
        {
            return this->m_data;
        }
        [[nodiscard]] T const* data() const noexcept
        {
            return this->m_data;
        }
    };

    struct SafeGlobalMemoryHandleTraits final
    {
        static constexpr HGLOBAL Invalid() noexcept
        {
            return nullptr;
        }

        static constexpr bool IsValid(HGLOBAL handle) noexcept
        {
            return handle != nullptr;
        }

        static void Reset(HGLOBAL handle, size_t) noexcept
        {
            if (handle != nullptr)
            {
                GlobalFree(handle);
            }
        }
    };

    using SafeGlobalMemoryHandle = SafeHandleT<HGLOBAL, SafeGlobalMemoryHandleTraits>;
}
