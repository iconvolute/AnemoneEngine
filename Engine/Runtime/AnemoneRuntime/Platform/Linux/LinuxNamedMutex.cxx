#include "AnemoneRuntime/Platform/NamedMutex.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxNamedMutex.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"

#include <fcntl.h>
#include <sys/stat.h>
#include <fmt/format.h>
#include <utility>

namespace Anemone
{
    NamedMutex::NamedMutex(std::string_view name)
    {
        std::string formatted = fmt::format("/anemone-engine-named-mutex-{}", name);

        this->_handle = Internal::NativeNamedMutex{sem_open(formatted.c_str(), O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1)};

        if (this->_handle.Value() == SEM_FAILED)
        {
            AE_PANIC("Failed to create named mutex: {}", errno);
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
            this->_handle = std::exchange(other._handle, {});
        }

        return *this;
    }

    NamedMutex::~NamedMutex() = default;

    void NamedMutex::Lock()
    {
        AE_ASSERT(this->_handle);

        auto& error = errno;

        int rc;

        do
        {
            rc = sem_wait(this->_handle.Value());
        } while (rc && (error == EINTR));

        if (rc)
        {
            AE_PANIC("Failed to lock named mutex: {}", error);
        }
    }

    bool NamedMutex::TryLock()
    {
        AE_ASSERT(this->_handle);

        return sem_trywait(this->_handle.Value()) == 0;
    }

    void NamedMutex::Unlock()
    {
        AE_ASSERT(this->_handle);

        if (sem_post(this->_handle.Value()))
        {
            AE_PANIC("Failed to unlock named mutex: {}", errno);
        }
    }
}
