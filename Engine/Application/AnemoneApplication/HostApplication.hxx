#pragma once
#include "AnemoneInterop/Headers.hxx"
#include "AnemoneBase/Reference.hxx"
#include "AnemoneApplication/HostWindow.hxx"

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
        Math::RectF DisplayRect;
        Math::RectF WorkAreaRect;
    };

    struct DisplayMetrics
    {
        std::vector<DisplayInfo> Displays;
        Math::RectF VirtualDisplayRect;
        Math::RectF PrimaryDisplayWorkArea;
        Math::SizeF PrimaryDisplaySize;
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
        APPLICATION_API static void Initialize(HostApplicationEvents& events);
        APPLICATION_API static void Finalize();
        APPLICATION_API static HostApplication& Get();

    public:
        virtual void ProcessMessages() = 0;

        virtual Reference<HostWindow> MakeWindow(WindowType windowType, WindowMode windowMode) = 0;

        APPLICATION_API static void GetDisplayMetrics(DisplayMetrics& metrics);

        APPLICATION_API static ColorRef GetScreenPixel(Math::PointF position, float gamma);
    };
}
