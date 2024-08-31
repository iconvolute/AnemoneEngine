#pragma once

#if true
namespace Anemone::Diagnostic
{
}
#endif

#include <cstdlib>

#define AE_ASSERT(...)
#define AE_BUGCHECK(...) abort();
#define AE_FATAL(...)
#define AE_ENSURE(...)
