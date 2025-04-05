#include "AnemoneRuntime/Instant.hxx"
#include "AnemoneRuntime/System/Environment.hxx"

namespace Anemone
{
    Instant Instant::Now()
    {
        return Environment::GetCurrentInstant();
    }
}
