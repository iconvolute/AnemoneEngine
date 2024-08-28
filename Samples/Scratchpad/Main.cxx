#include "AnemoneRuntime/Instant.hxx"
#include "AnemoneRuntime/Platform/Main.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"
#include "AnemoneRuntime/Threading/Yielding.hxx"
#include "AnemoneRuntime/Platform/Runtime.hxx"

#include <fmt/format.h>


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
            fmt::println("Hello at {}", Anemone::Instant::Now().SinceEpoch());
        }

        Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));
        {
            AE_PROFILE_SCOPE(Outer);
            Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));
            {
                AE_PROFILE_SCOPE(Inner);
                Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));

                fmt::println("Crashed!?");
            }
            Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));
        }
        Anemone::Threading::ThisThread::Sleep(Anemone::Duration::FromMilliseconds(500));
    }
    return 0;
}
