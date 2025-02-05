#include "AnemoneRuntime/Threading/Semaphore.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#include "AnemoneRuntime/Platform/Unix/UnixSafeHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone
{
    class PthreadSemaphore final
    {
    private:
        int32_t _count;
        int32_t _waiting;
        pthread_mutex_t _mutex;
        pthread_cond_t _cond;

    public:
        PthreadSemaphore(int32_t initial)
            : _count{initial}
            , _waiting{0}
        {
            AE_ASSERT(initial >= 0);

            pthread_mutex_init(&this->_mutex, nullptr);

            pthread_condattr_t attr;
            pthread_condattr_init(&attr);
            pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
            pthread_cond_init(&this->_cond, &attr);
            pthread_condattr_destroy(&attr);
        }

        PthreadSemaphore(PthreadSemaphore const&) = delete;
        PthreadSemaphore(PthreadSemaphore&&) = delete;
        PthreadSemaphore& operator=(PthreadSemaphore const&) = delete;
        PthreadSemaphore& operator=(PthreadSemaphore&&) = delete;

        ~PthreadSemaphore()
        {
            pthread_cond_destroy(&this->_cond);
            pthread_mutex_destroy(&this->_mutex);
        }

    public:
        void Acquire()
        {
            pthread_mutex_lock(&this->_mutex);

            this->Wait();

            // Acquire the semaphore.
            --this->_count;

            pthread_mutex_unlock(&this->_mutex);
        }

        bool TryAcquire(Duration const& timeout)
        {
            // Determine timeout clock value.
            timespec ts{};
            clock_gettime(CLOCK_MONOTONIC, &ts);
            Interop::posix_ValidateTimeout(ts, timeout);

            pthread_mutex_lock(&this->_mutex);

            int rc = 0;

            // Try to acquire the semaphore.
            while (this->_count < 1)
            {
                rc = pthread_cond_timedwait(&this->_cond, &this->_mutex, &ts);

                if (rc == ETIMEDOUT)
                {
                    // Timeout occurred.
                    break;
                }
            }

            if (rc == 0)
            {
                // Semaphore acquired within the timeout.
                --this->_count;
            }

            pthread_mutex_unlock(&this->_mutex);

            return rc == 0;
        }

        bool TryAcquire()
        {
            pthread_mutex_lock(&this->_mutex);

            bool acquired = this->_count > 0;

            if (acquired)
            {
                // Acquire the semaphore.
                --this->_count;
            }

            pthread_mutex_unlock(&this->_mutex);

            return acquired;
        }

        void Release(int32_t count = 1)
        {
            if (count == 0)
            {
                return;
            }

            pthread_mutex_lock(&this->_mutex);

            ++this->_count;

            int32_t const waiting = this->_waiting;

            if (waiting == 0)
            {
                // No waiting threads.
            }
            else if (waiting <= count)
            {
                // Releasing more threads than waiters.
                pthread_cond_broadcast(&this->_cond);
            }
            else
            {
                // Releasing fewer threads than waiters.
                for (int32_t i = 0; i < count; ++i)
                {
                    pthread_cond_signal(&this->_cond);
                }
            }

            pthread_mutex_unlock(&this->_mutex);
        }

    private:
        void Wait()
        {
            ++this->_waiting;

            while (this->_count < 1)
            {
                pthread_cond_wait(&this->_cond, &this->_mutex);
            }

            --this->_waiting;
        }
    };

    //using Semaphore = PthreadSemaphore;
}

namespace Anemone
{
    class UnixSemaphore final
    {
    private:
        sem_t _inner;

    public:
        UnixSemaphore(int32_t initial)
        {
            AE_ASSERT(initial >= 0);

            sem_init(&this->_inner, 0, static_cast<unsigned int>(initial));
        }

        UnixSemaphore(UnixSemaphore const&) = delete;
        UnixSemaphore(UnixSemaphore&&) = delete;
        UnixSemaphore& operator=(UnixSemaphore const&) = delete;
        UnixSemaphore& operator=(UnixSemaphore&&) = delete;

        ~UnixSemaphore()
        {
            sem_destroy(&this->_inner);
        }

    public:
        void Acquire()
        {
            int rc;

            // TODO: Do some helper macro / function to handle this pattern
            do
            {
                rc = sem_wait(&this->_inner);
            } while (rc && (errno == EINTR));
        }

        bool TryAcquire(Duration const& timeout)
        {
            timespec ts{};
            clock_gettime(CLOCK_REALTIME, &ts);
            Interop::posix_ValidateTimeout(ts, timeout);

            int rc = 0;

            // Try to acquire the semaphore.
            do
            {
                rc = sem_timedwait(&this->_inner, &ts);
            } while (rc && (errno == EINTR));

            // Fail when ETIMEDOUT.
            return rc == 0;
        }

        bool TryAcquire()
        {
            return sem_trywait(&this->_inner) == 0;
        }

        void Release(int32_t count = 1)
        {
            for (; count != 0; --count)
            {
                sem_post(&this->_inner);
            }
        }
    };

    using Semaphore = UnixSemaphore;
}
