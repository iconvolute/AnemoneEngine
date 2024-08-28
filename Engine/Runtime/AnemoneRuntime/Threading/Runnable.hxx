#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

namespace Anemone::Threading
{
    //! Represents a runnable object.
    //!
    //! A runnable object is an object that can be executed by a thread.
    //! It is possible to run a runnable object multiple times.
    class RUNTIME_API Runnable
    {
    public:
        Runnable() = default;
        Runnable(Runnable const&) = default;
        Runnable(Runnable&&) = default;
        Runnable& operator=(Runnable const&) = default;
        Runnable& operator=(Runnable&&) = default;
        virtual ~Runnable();

    protected:
        //! Called when the runnable object is started.
        //! \return True if the runnable object was successfully started, false otherwise.
        virtual bool OnStart();

        //! Called when the runnable object is run.
        virtual void OnRun() = 0;

        //! Called when the runnable object is finished.
        virtual void OnFinish();

    public:
        void Run();
    };
}
