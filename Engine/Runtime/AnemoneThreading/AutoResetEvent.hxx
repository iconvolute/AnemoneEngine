#pragma once
#include "AnemoneThreading/ConditionVariable.hxx"
#include "AnemoneThreading/CriticalSection.hxx"

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
            UniqueLock scope{this->_cs};

            while (!this->_set)
            {
                this->_cv.Wait(scope);
            }

            this->_set = false;
        }

        void Set()
        {
            {
                UniqueLock scope{this->_cs};
                this->_set = true;
            }

            this->_cv.NotifyOne();
        }
    };
}
