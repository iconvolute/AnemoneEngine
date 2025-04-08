#pragma once
#include "AnemoneRuntime/Threading/ConditionVariable.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"

namespace Anemone
{
    class ManualResetEvent final
    {
    private:
        CriticalSection _cs{};
        ConditionVariable _cv{};
        bool _set{false};

    public:
        ManualResetEvent() = default;

        explicit ManualResetEvent(bool signaled)
            : _set{signaled}
        {
        }

        ManualResetEvent(ManualResetEvent const&) = delete;

        ManualResetEvent(ManualResetEvent&&) = delete;

        ManualResetEvent& operator=(ManualResetEvent const&) = delete;

        ManualResetEvent& operator=(ManualResetEvent&&) = delete;

        ~ManualResetEvent() = default;

    public:
        void Wait()
        {
            UniqueLock scope{this->_cs};

            while (!this->_set)
            {
                this->_cv.Wait(lock);
            }
        }

        void Set()
        {
            {
                UniqueLock scope{this->_cs};
                this->_set = true;
            }

            this->_cv.NotifyAll();
        }

        void Reset()
        {
            UniqueLock scope{this->_cs};
            this->_set = false;
        }
    };
}
