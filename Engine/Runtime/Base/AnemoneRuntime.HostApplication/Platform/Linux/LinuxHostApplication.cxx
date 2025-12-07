#include "AnemoneRuntime.HostApplication/Platform/Linux/LinuxHostApplication.hxx"
#include "AnemoneRuntime.Base/UninitializedObject.hxx"

namespace Anemone
{
    namespace
    {
        UninitializedObject<LinuxHostApplication> gLinuxHostApplication{};
    }

    void HostApplication::Initialize(HostApplicationEvents& events)
    {
        gLinuxHostApplication.Create(events);
    }

    void HostApplication::Finalize()
    {
        gLinuxHostApplication.Destroy();
    }

    HostApplication& HostApplication::Get()
    {
        return *gLinuxHostApplication;
    }
}

namespace Anemone
{
    LinuxHostApplication::LinuxHostApplication(HostApplicationEvents& e)
        : events{&e}
    {
        AE_PANIC("Not implemented");
    }

    LinuxHostApplication::~LinuxHostApplication() = default;

    void LinuxHostApplication::ProcessMessages()
    {
        AE_PANIC("Not implemented");
    }

    Reference<HostWindow> LinuxHostApplication::MakeWindow(WindowType windowType, WindowMode windowMode)
    {
        AE_PANIC("Not implemented");
        (void)windowType;
        (void)windowMode;
        return {};
    }
}

namespace Anemone
{
    void HostApplication::GetDisplayMetrics(DisplayMetrics& metrics)
    {
        (void)metrics;
    }

    ColorRef HostApplication::GetScreenPixel(PointF position, float gamma)
    {
        (void)position;
        (void)gamma;
        return {};
    }
}
