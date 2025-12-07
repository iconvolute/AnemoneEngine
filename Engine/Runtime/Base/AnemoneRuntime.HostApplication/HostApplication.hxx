#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"
#include "AnemoneRuntime.Base/Reference.hxx"
#include "AnemoneRuntime.HostApplication/HostWindow.hxx"

namespace Anemone
{
    struct ColorRef final
    {
        float Red;
        float Green;
        float Blue;
        float Alpha;
    };

    enum class DisplayOrientation : uint8_t
    {
        Unknown,
        Landscape,
        LandscapeFlipped,
        Portrait,
        PortraitFlipped,
    };

    struct DisplayInfo
    {
        std::string Name;
        std::string Id;
        DisplayOrientation Orientation;
        bool Primary;
        RectF DisplayRect;
        RectF WorkAreaRect;
    };

    struct DisplayMetrics
    {
        std::vector<DisplayInfo> Displays;
        RectF VirtualDisplayRect;
        RectF PrimaryDisplayWorkArea;
        SizeF PrimaryDisplaySize;
    };
}

namespace Anemone
{
    class HostApplicationEvents;

    class HostApplication
    {
    public:
        HostApplication() = default;
        HostApplication(HostApplication const&) = delete;
        HostApplication(HostApplication&&) = delete;
        virtual ~HostApplication() = default;
        HostApplication& operator=(HostApplication const&) = delete;
        HostApplication& operator=(HostApplication&&) = delete;

    public:
        ANEMONE_RUNTIME_BASE_API static void Initialize(HostApplicationEvents& events);
        ANEMONE_RUNTIME_BASE_API static void Finalize();
        ANEMONE_RUNTIME_BASE_API static HostApplication& Get();

    public:
        virtual void ProcessMessages() = 0;

        virtual Reference<HostWindow> MakeWindow(WindowType windowType, WindowMode windowMode) = 0;

        ANEMONE_RUNTIME_BASE_API static void GetDisplayMetrics(DisplayMetrics& metrics);

        ANEMONE_RUNTIME_BASE_API static ColorRef GetScreenPixel(PointF position, float gamma);
    };
}
