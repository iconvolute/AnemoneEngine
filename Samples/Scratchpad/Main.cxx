#include "AnemoneRuntime/Instant.hxx"
#include "AnemoneRuntime/Platform/Main.hxx"

#include <fmt/format.h>

int main()
{
    fmt::println("Hello at {}", Anemone::Instant::Now().SinceEpoch());
    return 0;
}
