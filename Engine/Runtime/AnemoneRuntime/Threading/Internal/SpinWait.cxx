#include "AnemoneRuntime/Threading/SpinWait.hxx"
#include "AnemoneRuntime/Threading/CurrentThread.hxx"

namespace Anemone
{
    void SpinWait::SpinOnce()
    {
        if (this->NextSpinWillYield())
        {
            uint32_t const count = this->m_Counter - SpinYieldThreshold;

            if ((count % 20) == 19)
            {
                CurrentThread::YieldAnyThreadOnAnyProcessor();
            }
            else if ((count % 5) == 4)
            {
                CurrentThread::YieldSameOrHigherPriorityThreadOnAnyProcessor();
            }
            else
            {
                CurrentThread::YieldAnyThreadOnSameProcessor();
            }
        }
        else
        {
            size_t const count = size_t{2} << this->m_Counter;

            for (size_t i = 0; i < count; ++i)
            {
                CurrentThread::Pause();
            }
        }

        ++this->m_Counter;

        if (this->m_Counter == 0)
        {
            this->m_Counter = SpinYieldThreshold;
        }
    }
}
