#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

namespace Anemone
{
    namespace
    {
        struct InputStatics final
        {
        };

        UninitializedObject<InputStatics> gInputStatics{};
    }
}

namespace Anemone::Internal
{
    extern void InitializeInput()
    {
        gInputStatics.Create();
    }

    extern void FinalizeInput()
    {
        gInputStatics.Destroy();
    }
}
