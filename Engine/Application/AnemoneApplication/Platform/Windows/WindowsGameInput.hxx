#pragma once
#include "AnemoneApplication/HostApplicationEvents.hxx"
#include "AnemoneInterop/Windows/UI.hxx"
#include "AnemoneInterop/Windows/SafeHandle.hxx"

#include <GameInput.h>
#include <wrl/client.h>

namespace Anemone
{
    class Window;
}

namespace Anemone
{
    class WindowsGameInput final
    {
    public:
        static WindowsGameInput& Get();
        static void Initialize();
        static void Finalize();

    private:
        Microsoft::WRL::ComPtr<IGameInput> m_gameInput{};
        GameInputCallbackToken m_token{};
        Interop::Windows::SafeHandle m_scrollWheelEvent{};
        size_t m_mouseCount{};

        struct State final
        {
            MouseButtons Buttons{};
            float PositionX{};
            float PositionY{};
            float Scroll{};
        };

        State m_state{};

        static void CALLBACK OnGameInputDevice(
            GameInputCallbackToken callbackToken,
            void* context,
            IGameInputDevice* device,
            uint64_t timestamp,
            GameInputDeviceStatus currentStatus,
            GameInputDeviceStatus previousStatus);

    public:
        WindowsGameInput();
        WindowsGameInput(WindowsGameInput const&) = delete;
        WindowsGameInput(WindowsGameInput&&) = delete;
        WindowsGameInput& operator=(WindowsGameInput const&) = delete;
        WindowsGameInput& operator=(WindowsGameInput&&) = delete;
        ~WindowsGameInput();

        void Poll(HostApplicationEvents& events);
    };
}
