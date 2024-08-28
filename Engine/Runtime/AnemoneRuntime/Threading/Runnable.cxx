#include "AnemoneRuntime/Threading/Runnable.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"

namespace Anemone::Threading
{
    Runnable::~Runnable() = default;

    bool Runnable::OnStart()
    {
        return true;
    }

    void Runnable::OnRun()
    {
        AE_BUGCHECK("Runnable::OnRun must be overridden");
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
