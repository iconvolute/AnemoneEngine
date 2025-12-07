#include "AnemoneRuntime.System/Platform/NamedMutex.hxx"
#include "AnemoneRuntime.System/Platform/Windows/WindowsNamedMutex.hxx"
#include "AnemoneRuntime.Interop/Windows/FileSystem.hxx"
#include "AnemoneRuntime.Interop/Windows/Text.hxx"
#include "AnemoneRuntime.Diagnostics/Debug.hxx"

namespace Anemone
{
    std::expected<WindowsNamedMutex, Error> WindowsNamedMutex::Create(std::string_view name)
    {
        std::string sname = fmt::format("Global\\anemone-engine-named-mutex-{}", name);
        Interop::Windows::FilePathW wname{};
        Interop::Windows::WidenString(wname, sname);

        Interop::Windows::SafeHandle handle{CreateMutexW(nullptr, FALSE, wname.data())};

        if (handle)
        {
            return WindowsNamedMutex{std::move(handle)};
        }


        return std::unexpected(Error::InvalidOperation);
    }

    void WindowsNamedMutex::Lock()
    {
        AE_ASSERT(this->_handle);

        DWORD const dwResult = WaitForSingleObject(this->_handle.Get(), INFINITE);

        if (dwResult != WAIT_OBJECT_0)
        {
            AE_PANIC("Failed to lock named mutex");
        }
    }

    bool WindowsNamedMutex::TryLock()
    {
        AE_ASSERT(this->_handle);

        DWORD const dwResult = WaitForSingleObject(this->_handle.Get(), 0);

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
        AE_ASSERT(this->_handle);

        if (not ReleaseMutex(this->_handle.Get()))
        {
            AE_PANIC("Failed to unlock named mutex");
        }
    }
}
