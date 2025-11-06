#include "AnemoneBase/UninitializedObject.hxx"
#include "AnemoneInterop/Windows/UI.hxx"
#include "AnemoneInterop/Windows/Text.hxx"
#include "AnemoneInterop/Windows/Graphics.hxx"
#include "AnemoneApplication/Platform/Windows/WindowsHostApplication.hxx"
#include "AnemoneApplication/Platform/Windows/WindowsHostWindow.hxx"
#include "AnemoneApplication/Platform/Windows/WindowsHostInput.hxx"
#include "AnemoneApplication/Platform/Windows/WindowsGameInput.hxx"
#include "AnemoneApplication/Platform/Windows/WindowsXInput.hxx"

namespace Anemone
{
    namespace
    {
        UninitializedObject<WindowsHostApplication> gWindowsHostApplication{};
    }

    void HostApplication::Initialize(HostApplicationEvents& events)
    {
        gWindowsHostApplication.Create(events);
        WindowsHostInput::Initialize();
        WindowsGameInput::Initialize();
        WindowsXInput::Initialize();
    }

    void HostApplication::Finalize()
    {
        WindowsXInput::Finalize();
        WindowsGameInput::Finalize();
        WindowsHostInput::Finalize();
        gWindowsHostApplication.Destroy();
    }

    HostApplication& HostApplication::Get()
    {
        return *gWindowsHostApplication;
    }

    WindowsHostApplication::WindowsHostApplication(HostApplicationEvents& e)
        : events{&e}
    {
        using namespace Interop::Windows;

        this->applicationModule = GetModuleHandleW(nullptr);

        this->applicationIcon = LoadIconW(this->applicationModule, MAKEINTRESOURCEW(IDI_MAIN_ICON));

        this->cursorArrow = LoadCursorW(nullptr, IDC_ARROW);
        this->cursorArrowWait = LoadCursorW(nullptr, IDC_APPSTARTING);
        this->cursorText = LoadCursorW(nullptr, IDC_IBEAM);
        this->cursorSizeHorizontal = LoadCursorW(nullptr, IDC_SIZEWE);
        this->cursorSizeVertical = LoadCursorW(nullptr, IDC_SIZENS);
        this->cursorSizeLeft = LoadCursorW(nullptr, IDC_SIZEWE);
        this->cursorSizeTop = LoadCursorW(nullptr, IDC_SIZENS);
        this->cursorSizeRight = LoadCursorW(nullptr, IDC_SIZEWE);
        this->cursorSizeBottom = LoadCursorW(nullptr, IDC_SIZENS);
        this->cursorSizeTopLeft = LoadCursorW(nullptr, IDC_SIZENWSE);
        this->cursorSizeTopRight = LoadCursorW(nullptr, IDC_SIZENESW);
        this->cursorSizeBottomLeft = LoadCursorW(nullptr, IDC_SIZENESW);
        this->cursorSizeBottomRight = LoadCursorW(nullptr, IDC_SIZENWSE);
        this->cursorSizeAll = LoadCursorW(nullptr, IDC_SIZEALL);
        this->cursorCross = LoadCursorW(nullptr, IDC_CROSS);
    }

    WindowsHostApplication::~WindowsHostApplication() = default;

    HCURSOR WindowsHostApplication::GetCursor(CursorType cursor) const
    {
        switch (cursor)
        {
        case CursorType::None:
            return {};

        case CursorType::Arrow:
            return this->cursorArrow;

        case CursorType::ArrowWait:
            return this->cursorArrowWait;

        case CursorType::Text:
            return this->cursorText;

        case CursorType::SizeHorizontal:
            return this->cursorSizeHorizontal;

        case CursorType::SizeVertical:
            return this->cursorSizeVertical;

        case CursorType::SizeLeft:
            return this->cursorSizeLeft;

        case CursorType::SizeTop:
            return this->cursorSizeTop;

        case CursorType::SizeRight:
            return this->cursorSizeRight;

        case CursorType::SizeBottom:
            return this->cursorSizeBottom;

        case CursorType::SizeTopLeft:
            return this->cursorSizeTopLeft;

        case CursorType::SizeTopRight:
            return this->cursorSizeTopRight;

        case CursorType::SizeBottomLeft:
            return this->cursorSizeBottomLeft;

        case CursorType::SizeBottomRight:
            return this->cursorSizeBottomRight;

        case CursorType::SizeAll:
            return this->cursorSizeAll;

        case CursorType::Cross:
            return this->cursorCross;
        }

        return {};
    }

    void WindowsHostApplication::ProcessMessages()
    {
        // Process messages.
        MSG message{};

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != FALSE)
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        // Pool input devices.
        WindowsXInput::Get().Poll(*this->events);

        // Disable for now.
        //WindowsGameInput::Get().Poll(*this->events);
    }

    Reference<HostWindow> WindowsHostApplication::MakeWindow(WindowType windowType, WindowMode windowMode)
    {
        return MakeReference<WindowsHostWindow>(*this, windowType, windowMode);
    }
}

namespace Anemone
{
    void HostApplication::GetDisplayMetrics(DisplayMetrics& metrics)
    {
        metrics.Displays.clear();

        // Get display size.
        metrics.PrimaryDisplaySize = SizeF{
            .Width = static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
            .Height = static_cast<float>(GetSystemMetrics(SM_CYSCREEN)),
        };

        RECT workArea = {-1, -1, -1, -1};

        if (SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0) == FALSE)
        {
            workArea = {};
        }

        // Get workspace area.
        metrics.PrimaryDisplayWorkArea = Interop::Windows::ToRectF(workArea);

        // Virtual display rect
        metrics.VirtualDisplayRect = RectF{
            .X = static_cast<float>(GetSystemMetrics(SM_XVIRTUALSCREEN)),
            .Y = static_cast<float>(GetSystemMetrics(SM_YVIRTUALSCREEN)),
            .Width = static_cast<float>(GetSystemMetrics(SM_CXVIRTUALSCREEN)),
            .Height = static_cast<float>(GetSystemMetrics(SM_CYVIRTUALSCREEN)),
        };

        // Get displays
        metrics.Displays.clear();

        DISPLAY_DEVICEW device{};
        device.cb = sizeof(device);

        for (DWORD id = 0; EnumDisplayDevicesW(nullptr, id, &device, 0) != FALSE; ++id)
        {
            if ((device.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) != 0)
            {
                // Mirroring devices are not supported.
                continue;
            }

            if ((device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == 0)
            {
                // Only devices attached to desktop.
                continue;
            }

            DisplayInfo& display = metrics.Displays.emplace_back();
            Interop::Windows::NarrowString(display.Id, device.DeviceID);
            Interop::Windows::NarrowString(display.Name, device.DeviceName);
            display.Primary = (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;

            DEVMODEW dm{};
            dm.dmSize = sizeof(dm);

            if (EnumDisplaySettingsExW(
                    device.DeviceName,
                    ENUM_CURRENT_SETTINGS,
                    &dm,
                    EDS_RAWMODE))
            {
                if (dm.dmFields & DM_DISPLAYORIENTATION)
                {
                    switch (dm.dmDisplayOrientation)
                    {
                    default:
                        display.Orientation = DisplayOrientation::Unknown;
                        break;

                    case DMDO_DEFAULT:
                        display.Orientation = DisplayOrientation::Landscape;
                        break;

                    case DMDO_90:
                        display.Orientation = DisplayOrientation::Portrait;
                        break;

                    case DMDO_180:
                        display.Orientation = DisplayOrientation::LandscapeFlipped;
                        break;

                    case DMDO_270:
                        display.Orientation = DisplayOrientation::PortraitFlipped;
                        break;
                    }
                }
            }
            else
            {
                display.Orientation = DisplayOrientation::Unknown;
            }

            // Find detailed properties.
            (void)Interop::Windows::EnumDisplayMonitors(
                nullptr,
                nullptr,
                [&](HMONITOR handle, HDC dc, LPRECT rect) -> bool
            {
                (void)rect;
                (void)dc;

                MONITORINFOEXW miex{};
                miex.cbSize = sizeof(miex);

                GetMonitorInfoW(handle, &miex);

                Interop::string_buffer<char, 128> name{};
                Interop::Windows::NarrowString(name, miex.szDevice);

                AE_ASSERT(!metrics.Displays.empty());

                DisplayInfo& last = metrics.Displays.back();

                if (last.Name == name.as_view())
                {
                    last.DisplayRect = Interop::Windows::ToRectF(miex.rcMonitor);
                    last.WorkAreaRect = Interop::Windows::ToRectF(miex.rcWork);
                }

                return true;
            });
        }
    }

    ColorRef HostApplication::GetScreenPixel(PointF position, float gamma)
    {
        COLORREF const color = GetPixel(GetDC(HWND_DESKTOP), static_cast<int>(position.X), static_cast<int>(position.Y));

        float const r = static_cast<float>(static_cast<uint8_t>(color)) / 255.0f;
        float const g = static_cast<float>(static_cast<uint8_t>(color >> 8)) / 255.0f;
        float const b = static_cast<float>(static_cast<uint8_t>(color >> 16)) / 255.0f;

        float const exp = std::max(gamma, 1.0F);

        return ColorRef{
            .Red = std::pow(r, exp),
            .Green = std::pow(g, exp),
            .Blue = std::pow(b, exp),
            .Alpha = 1.0f,
        };
    }
}
