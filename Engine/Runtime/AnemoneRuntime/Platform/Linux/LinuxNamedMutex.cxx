#include "AnemoneRuntime/Platform/Linux/LinuxNamedMutex.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"

#include <fcntl.h>
#include <sys/stat.h>
#include <fmt/format.h>
#include <utility>

namespace Anemone
{
    LinuxNamedMutex::LinuxNamedMutex(std::string_view name)
    {
        std::string formatted = fmt::format("/anemone-engine-named-mutex-{}", name);

        this->_handle = sem_open(formatted.c_str(), O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);

        if (this->_handle == SEM_FAILED)
        {
            AE_PANIC("Failed to create named mutex: {}", errno);
        }
    }

    LinuxNamedMutex::LinuxNamedMutex(LinuxNamedMutex&& other) noexcept
        : _handle{std::exchange(other._handle, nullptr)}
    {
    }

    LinuxNamedMutex& LinuxNamedMutex::operator=(LinuxNamedMutex&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->_handle != nullptr)
            {
                sem_close(this->_handle);
            }

            this->_handle = std::exchange(other._handle, {});
        }

        return *this;
    }

    LinuxNamedMutex::~LinuxNamedMutex()
    {
        if (this->_handle != nullptr)
        {
            sem_close(this->_handle);
        }
    }

    void LinuxNamedMutex::Lock()
    {
        AE_ASSERT(this->_handle != nullptr);

        auto& error = errno;

        int rc;

        do
        {
            rc = sem_wait(this->_handle);
        } while (rc && (error == EINTR));

        if (rc)
        {
            AE_PANIC("Failed to lock named mutex: {}", error);
        }
    }

    bool LinuxNamedMutex::TryLock()
    {
        AE_ASSERT(this->_handle != nullptr);

        return sem_trywait(this->_handle) == 0;
    }

    void LinuxNamedMutex::Unlock()
    {
        AE_ASSERT(this->_handle != nullptr);

        if (sem_post(this->_handle))
        {
            AE_PANIC("Failed to unlock named mutex: {}", errno);
        }
    }
}
