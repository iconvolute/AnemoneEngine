#include "AnemoneRuntime/System/NamedMutex.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"

namespace Anemone::System
{
    NamedMutex::NamedMutex(std::string_view name)
    {
        this->_name = fmt::format("/anemone-engine-named-mutex-{}", name);

        Platform::NativeNamedMutex& nativeThis = Platform::Create(this->_native);

        sem_t* handle = sem_open(this->_name.c_str(), O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);

        if (handle == SEM_FAILED)
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
                sem_close(nativeThis.Handle);
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
            sem_close(nativeThis.Handle);
        }

        Platform::Destroy(this->_native);
    }

    void NamedMutex::Lock()
    {
        Platform::NativeNamedMutex& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Handle != nullptr);

        auto& error = errno;
        int rc;

        do
        {
            rc = sem_wait(nativeThis.Handle);
        } while (rc && error == EINTR);

        if (rc)
        {
            AE_BUGCHECK("Failed to lock named mutex.");
        }
    }

    bool NamedMutex::TryLock()
    {
        Platform::NativeNamedMutex& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Handle != nullptr);

        return sem_trywait(nativeThis.Handle) == 0;
    }

    void NamedMutex::Unlock()
    {
        Platform::NativeNamedMutex& nativeThis = Platform::Get(this->_native);
        AE_ASSERT(nativeThis.Handle != nullptr);

        if (sem_post(nativeThis.Handle))
        {
            AE_BUGCHECK("Failed to unlock named mutex.");
        }
    }
}
