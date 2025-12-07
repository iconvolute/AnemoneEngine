#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"
#include "AnemoneRuntime.Base/Reference.hxx"

#include <concepts>

namespace Anemone
{
    //! Represents a runnable object.
    //!
    //! A runnable object is an object that can be executed by a thread.
    //! It is possible to run a runnable object multiple times.
    class ANEMONE_RUNTIME_BASE_API Runnable : public ReferenceCounted<Runnable>
    {
    public:
        Runnable() = default;

        Runnable(Runnable const&) = delete;

        Runnable(Runnable&&) = delete;

        Runnable& operator=(Runnable const&) = delete;

        Runnable& operator=(Runnable&&) = delete;

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

    template <std::invocable Callback>
    Reference<Runnable> MakeRunnable(Callback callback)
    {
        struct Wrapper : Runnable
        {
            Callback _callback;

            void OnRun() override
            {
                this->_callback();
            }
        };

        return MakeReference<Wrapper>(std::move(callback));
    }
}
