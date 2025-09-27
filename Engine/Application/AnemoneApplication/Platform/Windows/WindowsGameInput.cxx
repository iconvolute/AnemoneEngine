#include "AnemoneApplication/Platform/Windows/WindowsGameInput.hxx"
#include "AnemoneApplication/Platform/Windows/WindowsHostWindow.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Windows/WindowsDebug.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

namespace Anemone
{
    namespace
    {
        Anemone::UninitializedObject<WindowsGameInput> gWindowsGameInput;
    }

    WindowsGameInput& WindowsGameInput::Get()
    {
        return *gWindowsGameInput;
    }

    void WindowsGameInput::Initialize()
    {
        gWindowsGameInput.Create();
    }

    void WindowsGameInput::Finalize()
    {
        gWindowsGameInput.Destroy();
    }
    void CALLBACK WindowsGameInput::OnGameInputDevice(
        GameInputCallbackToken callbackToken,
        void* context,
        IGameInputDevice* device,
        uint64_t timestamp,
        GameInputDeviceStatus currentStatus,
        GameInputDeviceStatus previousStatus)
    {
        (void)device;
        (void)callbackToken;
        (void)timestamp;

        WindowsGameInput* self = static_cast<WindowsGameInput*>(context);

        bool const isConnected = (currentStatus & GameInputDeviceConnected) != 0;
        bool const wasConnected = (previousStatus & GameInputDeviceConnected) != 0;

        if (isConnected and not wasConnected)
        {
            ++self->m_mouseCount;
        }
        else if (not isConnected and wasConnected and (self->m_mouseCount > 0))
        {
            --self->m_mouseCount;
        }
    }

    WindowsGameInput::WindowsGameInput()
    {
        HRESULT hr = GameInputCreate(this->m_gameInput.GetAddressOf());

        if (SUCCEEDED(hr))
        {
            hr = this->m_gameInput->RegisterDeviceCallback(
                nullptr,
                GameInputKindMouse,
                GameInputDeviceConnected,
                GameInputBlockingEnumeration,
                this,
                OnGameInputDevice,
                &this->m_token);

            if (SUCCEEDED(hr))
            {
                this->m_scrollWheelEvent.Attach(CreateEventExW(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE));

                if (!this->m_scrollWheelEvent.IsValid())
                {
                    AE_VERIFY_WIN32(GetLastError());
                }
            }
            else
            {
                AE_VERIFY_HRESULT(hr);
            }
        }
        else
        {
            AE_VERIFY_HRESULT(hr);
        }
    }

    WindowsGameInput::~WindowsGameInput()
    {
        if (this->m_token)
        {
            if (this->m_gameInput)
            {
                this->m_gameInput->UnregisterCallback(this->m_token, UINT64_MAX);
            }

            this->m_token = {};
        }
    }

    void WindowsGameInput::Poll(HostApplicationEvents& events)
    {
        // todo: implement pooling events
        if (this->m_gameInput)
        {
            Microsoft::WRL::ComPtr<IGameInputReading> reading{};

            if (SUCCEEDED(this->m_gameInput->GetCurrentReading(GameInputKindMouse, nullptr, reading.GetAddressOf())))
            {
                GameInputMouseState state{};

                if (reading->GetMouseState(&state))
                {
                    AE_TRACE(Error, "MB: {}", std::to_underlying(state.buttons));
                    AE_TRACE(Error, "PX: {}", state.positionX);
                    AE_TRACE(Error, "PY: {}", state.positionY);
                    AE_TRACE(Error, "WX: {}", state.wheelX);
                    AE_TRACE(Error, "WY: {}", state.wheelY);
                }
            }
        }

        (void)events;
    }
}
