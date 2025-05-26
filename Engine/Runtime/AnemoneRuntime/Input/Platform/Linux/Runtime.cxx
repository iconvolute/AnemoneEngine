#include "AnemoneRuntime/Interop/Headers.hxx"
#include "AnemoneRuntime/Input/Input.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

namespace Anemone
{
    struct InputStatics final
    {
    };

    namespace
    {
        UninitializedObject<InputStatics> GInputStatics{};
    }
}

namespace Anemone
{
    void Input::Initialize(Runtime::RuntimeInitializerContext const& context)
    {
        GInputStatics.Create();
    }

    void Input::Finalize(Runtime::RuntimeFinalizerContext const& context)
    {
        GInputStatics.Destroy();
    }
}
