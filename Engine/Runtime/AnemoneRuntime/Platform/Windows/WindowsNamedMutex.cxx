#include "AnemoneRuntime/Platform/Windows/WindowsNamedMutex.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    WindowsNamedMutex::WindowsNamedMutex(std::string_view name)
    {
        // TODO: Refactor this into something more convenient to use
        std::string sname = fmt::format("Global\\anemone-engine-named-mutex-{}", name);

        Interop::win32_FilePathW wname{};
        Interop::win32_WidenString(wname, sname);

        this->_handle = CreateMutexW(nullptr, FALSE, wname.data());

        if (this->_handle == nullptr)
        {
            AE_PANIC("Failed to create named mutex");
        }
    }

    WindowsNamedMutex::WindowsNamedMutex(WindowsNamedMutex&& other) noexcept
        : _handle{std::exchange(other._handle, {})}
    {
        
    }

    WindowsNamedMutex& WindowsNamedMutex::operator=(WindowsNamedMutex&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->_handle != nullptr)
            {
                CloseHandle(this->_handle);
            }

            this->_handle = std::exchange(other._handle, {});
        }

        return *this;
    }

    WindowsNamedMutex::~WindowsNamedMutex()
    {
        if (this->_handle != nullptr)
        {
            CloseHandle(this->_handle);
        }
    }

    void WindowsNamedMutex::Lock()
    {
        AE_ASSERT(this->_handle != nullptr);

        DWORD const dwResult = WaitForSingleObject(this->_handle, INFINITE);

        if (dwResult != WAIT_OBJECT_0)
        {
            AE_PANIC("Failed to lock named mutex");
        }
    }

    bool WindowsNamedMutex::TryLock()
    {
        AE_ASSERT(this->_handle != nullptr);

        DWORD const dwResult = WaitForSingleObject(this->_handle, 0);

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

    void WindowsNamedMutex::Unlock()
    {
        AE_ASSERT(this->_handle != nullptr);

        if (not ReleaseMutex(this->_handle))
        {
            AE_PANIC("Failed to unlock named mutex");
        }
    }
}
