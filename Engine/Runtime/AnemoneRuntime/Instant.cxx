#include "AnemoneRuntime/Instant.hxx"
#include "AnemoneRuntime/Platform/Environment.hxx"

namespace Anemone
{
    Instant Instant::Now()
    {
        return Environment::GetCurrentInstant();
    }
}
