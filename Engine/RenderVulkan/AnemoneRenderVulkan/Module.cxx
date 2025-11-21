#include "AnemoneRenderVulkan/Module.hxx"
#include "AnemoneRenderVulkan/VulkanInstance.hxx"

namespace Anemone
{
    void Module_RenderVulkan::Initialize()
    {
        VulkanInstance::StaticInitialize();
    }

    void Module_RenderVulkan::Finalize()
    {
        VulkanInstance::StaticFinalize();
    }
}
