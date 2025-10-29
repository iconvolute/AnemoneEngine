#include "AnemoneThreading/Runnable.hxx"
#include "AnemoneDiagnostics/Debug.hxx"

namespace Anemone
{
    Runnable::~Runnable() = default;

    bool Runnable::OnStart()
    {
        return true;
    }

    void Runnable::OnRun()
    {
        AE_PANIC("Runnable::OnRun must be overridden");
    }

    void Runnable::OnFinish()
    {
    }

    void Runnable::Run()
    {
        if (this->OnStart())
        {
            this->OnRun();
            this->OnFinish();
        }
    }
}
