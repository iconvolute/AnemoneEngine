#pragma once
#include "AnemoneRuntime/Threading/ConditionVariable.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"

namespace Anemone
{
    class AutoResetEvent final
    {
    private:
        CriticalSection _cs{};
        ConditionVariable _cv{};
        bool _set{false};

    public:
        AutoResetEvent() = default;

        explicit AutoResetEvent(bool signaled)
            : _set{signaled}
        {
        }

        AutoResetEvent(AutoResetEvent const&) = delete;

        AutoResetEvent(AutoResetEvent&&) = delete;

        AutoResetEvent& operator=(AutoResetEvent const&) = delete;

        AutoResetEvent& operator=(AutoResetEvent&&) = delete;

        ~AutoResetEvent() = default;

    public:
        void Wait()
        {
            UniqueLock _{this->_cs};

            while (!this->_set)
            {
                this->_cv.Wait(this->_cs);
            }

            this->_set = false;
        }

        void Set()
        {
            {
                UniqueLock _{this->_cs};
                this->_set = true;
            }

            this->_cv.NotifyOne();
        }
    };
}
