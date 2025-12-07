#pragma once
#include "AnemoneRuntime.Threading/Thread.hxx"
#include "AnemoneRuntime.Base/Duration.hxx"

namespace Anemone
{
    struct CurrentThread final
    {
        ANEMONE_RUNTIME_BASE_API static void YieldAnyThreadOnAnyProcessor();
        ANEMONE_RUNTIME_BASE_API static void YieldAnyThreadOnSameProcessor();
        ANEMONE_RUNTIME_BASE_API static void YieldSameOrHigherPriorityThreadOnAnyProcessor();
        ANEMONE_RUNTIME_BASE_API static void Yield();
        ANEMONE_RUNTIME_BASE_API static void Sleep(int32_t milliseconds);
        ANEMONE_RUNTIME_BASE_API static void Sleep(Duration const& timeout);
        ANEMONE_RUNTIME_BASE_API static void Pause();
        ANEMONE_RUNTIME_BASE_API static ThreadId Id();
    };
}
