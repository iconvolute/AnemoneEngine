#pragma once
#include "AnemoneRuntime/Threading/ConditionVariable.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"

namespace Anemone
{
    class RUNTIME_API AutoResetEvent final
    {
    private:
        CriticalSection _cs{};
        ConditionVariable _cv{};
        bool _set{false};

    public:
        AutoResetEvent() = default;
        ~AutoResetEvent() = default;
        AutoResetEvent(AutoResetEvent const&) = delete;
        AutoResetEvent(AutoResetEvent&&) = delete;
        AutoResetEvent& operator=(AutoResetEvent const&) = delete;
        AutoResetEvent& operator=(AutoResetEvent&&) = delete;

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

            this->_cv.Notify();
        }
    };

    class RUNTIME_API ManualResetEvent final
    {
    private:
        CriticalSection _cs{};
        ConditionVariable _cv{};
        bool _set{false};

    public:
        ManualResetEvent() = default;
        ~ManualResetEvent() = default;
        ManualResetEvent(ManualResetEvent const&) = delete;
        ManualResetEvent(ManualResetEvent&&) = delete;
        ManualResetEvent& operator=(ManualResetEvent const&) = delete;
        ManualResetEvent& operator=(ManualResetEvent&&) = delete;

        void Wait()
        {
            UniqueLock _{this->_cs};

            while (!this->_set)
            {
                this->_cv.Wait(this->_cs);
            }
        }

        void Set()
        {
            {
                UniqueLock _{this->_cs};
                this->_set = true;
            }

            this->_cv.NotifyAll();
        }

        void Reset()
        {
            UniqueLock _{this->_cs};
            this->_set = false;
        }
    };
}
