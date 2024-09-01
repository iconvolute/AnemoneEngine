#include "AnemoneRuntime/System/Static.hxx"

namespace Anemone::System
{
    UninitializedObject<Environment> GEnvironment;
    UninitializedObject<ProcessorProperties> GProcessorProperties;

    void GenericSystemStatic::Initialize()
    {
        GEnvironment.Create();
        GProcessorProperties.Create();
    }

    void GenericSystemStatic::Finalize()
    {
        GProcessorProperties.Destroy();
        GEnvironment.Destroy();
    }
}
