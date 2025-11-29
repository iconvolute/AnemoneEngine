#pragma once
#include "AnemoneRenderVulkan/VulkanHeaders.hxx"
#include "AnemoneBase/Reference.hxx"
#include "AnemoneRender/Gpu.hxx"

namespace Anemone
{
    class VulkanDevice;

    class VulkanVertexShader final : public GpuVertexShader
    {
    public:
        static constexpr GpuShaderStage ShaderStage = GpuShaderStage::Vertex;

        VkShaderModule m_handle{};
        VulkanDevice* m_device{};

        explicit VulkanVertexShader(VulkanDevice& device)
            : m_device{&device}
        {
        }

        ~VulkanVertexShader() override;
    };

    class VulkanTessellationControlShader final : public GpuTessellationControlShader
    {
    public:
        static constexpr GpuShaderStage ShaderStage = GpuShaderStage::TessellationControl;

        VkShaderModule m_handle{};
        VulkanDevice* m_device{};

        explicit VulkanTessellationControlShader(VulkanDevice& device)
            : m_device{&device}
        {
        }

        ~VulkanTessellationControlShader() override;
    };

    class VulkanTessellationEvaluationShader final : public GpuTessellationEvaluationShader
    {
    public:
        static constexpr GpuShaderStage ShaderStage = GpuShaderStage::TessellationEvaluation;

        VkShaderModule m_handle{};
        VulkanDevice* m_device{};

        explicit VulkanTessellationEvaluationShader(VulkanDevice& device)
            : m_device{&device}
        {
        }

        ~VulkanTessellationEvaluationShader() override;
    };

    class VulkanGeometryShader final : public GpuGeometryShader
    {
    public:
        static constexpr GpuShaderStage ShaderStage = GpuShaderStage::Geometry;

        VkShaderModule m_handle{};
        VulkanDevice* m_device{};

        explicit VulkanGeometryShader(VulkanDevice& device)
            : m_device{&device}
        {
        }

        ~VulkanGeometryShader() override;
    };

    class VulkanFragmentShader final : public GpuFragmentShader
    {
    public:
        static constexpr GpuShaderStage ShaderStage = GpuShaderStage::Fragment;

        VkShaderModule m_handle{};
        VulkanDevice* m_device{};

        explicit VulkanFragmentShader(VulkanDevice& device)
            : m_device{&device}
        {
        }

        ~VulkanFragmentShader() override;
    };

    class VulkanMeshShader final : public GpuMeshShader
    {
    public:
        static constexpr GpuShaderStage ShaderStage = GpuShaderStage::Mesh;

        VkShaderModule m_handle{};
        VulkanDevice* m_device{};

        explicit VulkanMeshShader(VulkanDevice& device)
            : m_device{&device}
        {
        }

        ~VulkanMeshShader() override;
    };

    class VulkanTaskShader final : public GpuTaskShader
    {
    public:
        static constexpr GpuShaderStage ShaderStage = GpuShaderStage::Task;

        VkShaderModule m_handle{};
        VulkanDevice* m_device{};

        explicit VulkanTaskShader(VulkanDevice& device)
            : m_device{&device}
        {
        }

        ~VulkanTaskShader() override;
    };
}
