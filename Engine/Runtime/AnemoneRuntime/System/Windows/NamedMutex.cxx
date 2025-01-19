#include "AnemoneRuntime/System/NamedMutex.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::System
{
    NamedMutex::NamedMutex(std::string_view name)
        : m_native{}
    {
        this->m_name = fmt::format("Global\\anemone-engine-named-mutex-{}", name);

        Interop::win32_FilePathW wname{};
        Interop::win32_WidenString(wname, this->m_name);

        HANDLE const handle = CreateMutexW(nullptr, FALSE, wname.data());

        if (handle == nullptr)
        {
            AE_PANIC("Failed to create named mutex.");
        }
        else
        {
            this->m_native.Handle = handle;
        }
    }

    NamedMutex::NamedMutex(NamedMutex&& other) noexcept
        : m_native{std::exchange(other.m_native, {})}
        , m_name{std::move(other.m_name)}
    {
    }

    NamedMutex& NamedMutex::operator=(NamedMutex&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            this->m_name = std::exchange(other.m_name, {});

            if (this->m_native.Handle != nullptr)
            {
                CloseHandle(this->m_native.Handle);
            }

            this->m_native = std::exchange(other.m_native, {});
        }

        return *this;
    }

    NamedMutex::~NamedMutex()
    {
        if (this->m_native.Handle != nullptr)
        {
            CloseHandle(this->m_native.Handle);
        }
    }

    void NamedMutex::Lock()
    {
        AE_ASSERT(this->m_native.Handle != nullptr);

        DWORD const rc = WaitForSingleObject(this->m_native.Handle, INFINITE);

        if (rc != WAIT_OBJECT_0)
        {
            AE_PANIC("Failed to lock named mutex.");
        }
    }

    bool NamedMutex::TryLock()
    {
        AE_ASSERT(this->m_native.Handle != nullptr);

        DWORD const rc = WaitForSingleObject(this->m_native.Handle, 0);

        if (rc == WAIT_OBJECT_0)
        {
            return true;
        }

        if (rc == WAIT_TIMEOUT)
        {
            return false;
        }

        AE_PANIC("Failed to lock mutex");
    }

    void NamedMutex::Unlock()
    {
        AE_ASSERT(this->m_native.Handle != nullptr);

        if (not ReleaseMutex(this->m_native.Handle))
        {
            AE_PANIC("Failed to unlock named mutex.");
        }
    }
}
