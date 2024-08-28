#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Geometry.hxx"

#include <string_view>
#include <string>
#include <vector>

namespace Anemone::System
{
    RUNTIME_API bool GetEnvironmentVariable(std::string& result, std::string_view name);
    RUNTIME_API bool SetEnvironmentVariable(std::string_view name, std::string_view value);
    RUNTIME_API bool RemoveEnvironmentVariable(std::string_view name);
}

namespace Anemone::System
{
    RUNTIME_API bool GetClipboardContent(std::string& value);
    RUNTIME_API bool SetClipboardContent(std::string_view value);
}

namespace Anemone::System
{
    enum class DisplayOrientation : uint8_t
    {
        Unknown,
        Landscape,
        LandscapeFlipped,
        Portrait,
        PortraitFlipped,
    };

    struct DisplayInfo final
    {
        std::string Name;
        std::string Id;
        bool Primary;
        DisplayOrientation Orientation;
        Rectangle<int32_t> DisplayRectangle;
        Rectangle<int32_t> WorkAreaRectangle;
    };

    struct DisplayMetrics final
    {
        std::vector<DisplayInfo> Displays;
        Rectangle<int32_t> VirtualDisplayRect;
        Rectangle<int32_t> PrimaryDisplayWorkArea;
        Size<int32_t> PrimaryDisplaySize;
    };

    RUNTIME_API void GetDisplayMetrics(DisplayMetrics& displayMetrics);
}

namespace Anemone::System
{
    struct ColorRef final
    {
        float Red;
        float Green;
        float Blue;
        float Alpha;
    };

    [[nodiscard]] RUNTIME_API ColorRef GetScreenPixel(Point<int32_t> position, float gamma);
}

namespace Anemone::System
{
    struct ProcessorProperties
    {
        uint32_t PhysicalCores{};
        uint32_t LogicalCores{};
        uint32_t PerformanceCores{};
        uint32_t EfficiencyCores{};

        uint32_t CacheSizeLevel1{};
        uint32_t CacheSizeLevel2{};
        uint32_t CacheSizeLevel3{};
        uint32_t CacheSizeLevel4{};

        uint32_t CacheLineSize{};

        std::string Name;
        std::string Vendor;
    };

    RUNTIME_API ProcessorProperties const& GetProcessorProperties();
}
