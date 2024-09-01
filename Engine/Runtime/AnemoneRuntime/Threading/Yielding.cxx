#include "AnemoneRuntime/Threading/Yielding.hxx"

namespace Anemone::Threading
{
    void SpinWait::Spin()
    {
        if (this->NextSpinWillYield())
        {
            uint32_t const count = this->m_Counter - SpinYieldThreshold;

            if ((count % 20) == 19)
            {
                YieldThread(ThreadYieldTarget::AnyThreadOnAnyProcessor);
            }
            else if ((count % 5) == 4)
            {
                YieldThread(ThreadYieldTarget::SameOrHigherPriorityOnAnyProcessor);
            }
            else
            {
                YieldThread(ThreadYieldTarget::AnyThreadOnSameProcessor);
            }
        }
        else
        {
            size_t const count = size_t{2} << this->m_Counter;

            for (size_t i = 0; i < count; ++i)
            {
                PauseThread();
            }
        }

        ++this->m_Counter;

        if (this->m_Counter == 0)
        {
            this->m_Counter = SpinYieldThreshold;
        }
    }
}
