#include "AnemoneRuntime/Threading/Semaphore.hxx"

#if ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID

#include "AnemoneRuntime/Diagnostics/Assert.hxx"
#include "AnemoneRuntime/Interop/Linux/Threading.hxx"

namespace Anemone
{
    Semaphore::Semaphore(int32_t initial)
    {
        AE_ASSERT(initial >= 0);

        sem_init(&this->_inner, 0, static_cast<unsigned int>(initial));
    }

    Semaphore::~Semaphore()
    {
        sem_destroy(&this->_inner);
    }

    void Semaphore::Acquire()
    {
        int rc;

        // TODO: Do some helper macro / function to handle this pattern
        do
        {
            rc = sem_wait(&this->_inner);
        } while (rc && (errno == EINTR));
    }

    bool Semaphore::TryAcquire(Duration const& timeout)
    {
        timespec ts{};
        clock_gettime(CLOCK_REALTIME, &ts);
        Interop::Linux::ValidateTimeout(ts, timeout);

        int rc = 0;

        // Try to acquire the semaphore.
        do
        {
            rc = sem_timedwait(&this->_inner, &ts);
        } while (rc && (errno == EINTR));

        // Fail when ETIMEDOUT.
        return rc == 0;
    }

    bool Semaphore::TryAcquire()
    {
        return sem_trywait(&this->_inner) == 0;
    }

    void Semaphore::Release(int32_t count)
    {
        for (; count != 0; --count)
        {
            sem_post(&this->_inner);
        }
    }
}

#endif
