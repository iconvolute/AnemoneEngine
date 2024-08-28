#include "AnemoneRuntime/System/NamedMutex.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::System
{
    NamedMutex::NamedMutex(std::string_view name)
    {
        this->_name = fmt::format("Global\\anemone-engine-named-mutex-{}", name);

        Platform::NativeNamedMutex& nativeThis = Platform::Create(this->_native);

        Platform::win32_FilePathW wname{};
        Platform::win32_WidenString(wname, this->_name);

        HANDLE const handle = CreateMutexW(nullptr, FALSE, wname.data());

        if (handle == nullptr)
        {
            AE_BUGCHECK("Failed to create named mutex.");
        }
        else
        {
            nativeThis.Handle = handle;
        }
    }

    NamedMutex::NamedMutex(NamedMutex&& other) noexcept
        : _name{std::move(other._name)}
    {
        Platform::Create(this->_native, std::exchange(Platform::Get(other._native), {}));
    }

    NamedMutex& NamedMutex::operator=(NamedMutex&& other) noexcept
    {
        if (this != &other)
        {
            this->_name = std::move(other._name);

            Platform::NativeNamedMutex& nativeThis = Platform::Get(this->_native);
            Platform::NativeNamedMutex& nativeOther = Platform::Get(other._native);

            if (nativeThis.Handle != nullptr)
            {
                CloseHandle(nativeThis.Handle);
            }

            nativeThis = std::exchange(nativeOther, {});
        }

        return *this;
    }

    NamedMutex::~NamedMutex()
    {
        Platform::NativeNamedMutex& nativeThis = Platform::Get(this->_native);

        if (nativeThis.Handle != nullptr)
        {
            CloseHandle(nativeThis.Handle);
        }

        Platform::Destroy(this->_native);
    }

    void NamedMutex::Lock()
    {
        Platform::NativeNamedMutex& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Handle != nullptr);

        DWORD const rc = WaitForSingleObject(nativeThis.Handle, INFINITE);

        if (rc != WAIT_OBJECT_0)
        {
            AE_BUGCHECK("Failed to lock named mutex.");
        }
    }

    bool NamedMutex::TryLock()
    {
        Platform::NativeNamedMutex& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Handle != nullptr);

        DWORD const rc = WaitForSingleObject(nativeThis.Handle, 0);
        if (rc == WAIT_OBJECT_0)
        {
            return true;
        }

        if (rc == WAIT_TIMEOUT)
        {
            return false;
        }

        AE_BUGCHECK("Failed to lock mutex");
    }

    void NamedMutex::Unlock()
    {
        Platform::NativeNamedMutex& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Handle != nullptr);

        if (not ReleaseMutex(nativeThis.Handle))
        {
            AE_BUGCHECK("Failed to unlock named mutex.");
        }
    }
}
