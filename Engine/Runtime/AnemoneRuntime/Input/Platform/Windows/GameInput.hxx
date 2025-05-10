#pragma once
#include "AnemoneRuntime/Interop/Windows/UI.hxx"
#include "AnemoneRuntime/Input/Types.hxx"
#include "AnemoneRuntime/Interop/Windows/SafeHandle.hxx"
#include "AnemoneRuntime/App/IApplicationEvents.hxx"

#include <GameInput.h>
#include <wrl/client.h>

namespace Anemone
{
    class Window;
}

namespace Anemone
{
    class GameInput final
    {
    public:
        static void Initialize();
        static void Finalize();
        static GameInput& Get();

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
        GameInput();
        GameInput(GameInput const&) = delete;
        GameInput(GameInput&&) = delete;
        GameInput& operator=(GameInput const&) = delete;
        GameInput& operator=(GameInput&&) = delete;
        ~GameInput();

        void Poll(IApplicationEvents& events);
    };
}
