#pragma once
#include "AnemoneInterop/Headers.hxx"

namespace Anemone
{
    class Module_RenderVulkan
    {
    public:
        RENDER_VULKAN_API static void Initialize();
        RENDER_VULKAN_API static void Finalize();
    };
}
