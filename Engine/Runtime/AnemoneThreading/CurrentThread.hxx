#pragma once
#include "AnemoneThreading/Thread.hxx"
#include "AnemoneBase/Duration.hxx"

namespace Anemone
{
    struct CurrentThread final
    {
        RUNTIME_API static void YieldAnyThreadOnAnyProcessor();
        RUNTIME_API static void YieldAnyThreadOnSameProcessor();
        RUNTIME_API static void YieldSameOrHigherPriorityThreadOnAnyProcessor();
        RUNTIME_API static void Yield();
        RUNTIME_API static void Sleep(int32_t milliseconds);
        RUNTIME_API static void Sleep(Duration const& timeout);
        RUNTIME_API static void Pause();
        RUNTIME_API static ThreadId Id();
    };
}
