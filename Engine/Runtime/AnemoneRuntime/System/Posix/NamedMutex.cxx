#include "AnemoneRuntime/System/NamedMutex.hxx"
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

#include <utility>

namespace Anemone::System
{
    NamedMutex::NamedMutex(std::string_view name)
        : m_native{}
    {
        this->m_name = fmt::format("/anemone-engine-named-mutex-{}", name);

        sem_t* handle = sem_open(this->m_name.c_str(), O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);

        if (handle == SEM_FAILED)
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
        , m_name{std::exchange(other.m_name, {})}
    {
    }

    NamedMutex& NamedMutex::operator=(NamedMutex&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            this->m_name = std::exchange(other.m_name, {});

            if (this->m_native.Handle != nullptr)
            {
                sem_close(this->m_native.Handle);
            }

            this->m_native = std::exchange(other.m_native, {});
        }

        return *this;
    }

    NamedMutex::~NamedMutex()
    {
        if (this->m_native.Handle != nullptr)
        {
            sem_close(this->m_native.Handle);
        }
    }

    void NamedMutex::Lock()
    {
        AE_ASSERT(this->m_native.Handle != nullptr);

        auto& error = errno;
        int rc;

        do
        {
            rc = sem_wait(this->m_native.Handle);
        } while (rc && error == EINTR);

        if (rc)
        {
            AE_PANIC("Failed to lock named mutex.");
        }
    }

    bool NamedMutex::TryLock()
    {
        AE_ASSERT(this->m_native.Handle != nullptr);

        return sem_trywait(this->m_native.Handle) == 0;
    }

    void NamedMutex::Unlock()
    {
        AE_ASSERT(this->m_native.Handle != nullptr);

        if (sem_post(this->m_native.Handle))
        {
            AE_PANIC("Failed to unlock named mutex.");
        }
    }
}
