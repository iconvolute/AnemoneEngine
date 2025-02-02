#include "AnemoneRuntime/Platform/NamedMutex.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsNamedMutex.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    NamedMutex::NamedMutex(std::string_view name)
    {
        // TODO: Refactor this into something more convenient to use
        std::string sname = fmt::format("Global\\anemone-engine-named-mutex-{}", name);

        Interop::win32_FilePathW wname{};
        Interop::win32_WidenString(wname, sname);

        this->_handle = Internal::NativeNamedMutex{CreateMutexW(nullptr, FALSE, wname.data())};

        if (not this->_handle.IsValid())
        {
            AE_PANIC("Failed to create named mutex");
        }
    }

    NamedMutex::NamedMutex(NamedMutex&& other) noexcept
        : _handle{std::exchange(other._handle, {})}
    {
        
    }

    NamedMutex& NamedMutex::operator=(NamedMutex&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->_handle.IsValid())
            {
                CloseHandle(this->_handle.Value);
            }

            this->_handle = std::exchange(other._handle, Internal::NativeNamedMutex::Invalid());
        }

        return *this;
    }

    NamedMutex::~NamedMutex()
    {
        if (this->_handle.IsValid())
        {
            CloseHandle(this->_handle.Value);
        }
    }

    void NamedMutex::Lock()
    {
        AE_ASSERT(this->_handle.IsValid());

        DWORD const dwResult = WaitForSingleObject(this->_handle.Value, INFINITE);

        if (dwResult != WAIT_OBJECT_0)
        {
            AE_PANIC("Failed to lock named mutex");
        }
    }

    bool NamedMutex::TryLock()
    {
        AE_ASSERT(this->_handle.IsValid());

        DWORD const dwResult = WaitForSingleObject(this->_handle.Value, 0);

        if (dwResult == WAIT_OBJECT_0)
        {
            return true;
        }

        if (dwResult == WAIT_TIMEOUT)
        {
            return false;
        }

        AE_PANIC("Failed to lock named mutex");
        return false;
    }

    void NamedMutex::Unlock()
    {
        AE_ASSERT(this->_handle.IsValid());

        if (not ReleaseMutex(this->_handle.Value))
        {
            AE_PANIC("Failed to unlock named mutex");
        }
    }
}
