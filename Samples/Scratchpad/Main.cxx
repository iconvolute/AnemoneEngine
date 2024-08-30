#include "AnemoneRuntime/Instant.hxx"
#include "AnemoneRuntime/Platform/Main.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"
#include "AnemoneRuntime/Threading/Yielding.hxx"
#include "AnemoneRuntime/Platform/Runtime.hxx"
#include "AnemoneRuntime/Diagnostic/StackTrace.hxx"
#include "AnemoneRuntime/Diagnostic/Log.hxx"
#include "AnemoneRuntime/System/Environment.hxx"

#include <fmt/format.h>
#include <atomic>

AE_DECLARE_PROFILE(Main);
AE_DECLARE_PROFILE(Outer);
AE_DECLARE_PROFILE(Inner);

int main(int argc, char* argv[])
{
    Anemone::Platform::Runtime runtime{argc, argv};

    {
        AE_PROFILE_SCOPE(Main);
        {
            AE_PROFILE_SCOPE(Inner);
            AE_LOG(Warning, "Hello {}\n", Anemone::System::GetEnvironment().StartupTime);
            fmt::println("Hello at {}", Anemone::Instant::Now().SinceEpoch());
            Anemone::Diagnostic::GetCurrentStackTrace([&](auto const& entry)
            {
                AE_LOG(Warning, "{}\n", entry);
                return true;
            });
        }

        Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));
        {
            AE_PROFILE_SCOPE(Outer);
            Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));
            {
                AE_PROFILE_SCOPE(Inner);
                Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));

                AE_LOG(Fatal, "Crashed!?\n");
                std::atomic<int>* p = nullptr;
                p->store(42);
            }
            Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));
        }
        Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));
    }
    return 0;
}
