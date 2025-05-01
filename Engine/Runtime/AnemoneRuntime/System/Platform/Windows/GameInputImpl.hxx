#pragma once
#include "AnemoneRuntime/Interop/Windows/UI.hxx"
#include "AnemoneRuntime/Interop/Windows/SafeHandle.hxx"
#include "AnemoneRuntime/System/Application.hxx"

#include <GameInput.h>
#include <wrl/client.h>

namespace Anemone::Internal::Windows
{
    class GameInputImpl final
    {
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
        GameInputImpl();
        GameInputImpl(GameInputImpl const&) = delete;
        GameInputImpl(GameInputImpl&&) = delete;
        GameInputImpl& operator=(GameInputImpl const&) = delete;
        GameInputImpl& operator=(GameInputImpl&&) = delete;
        ~GameInputImpl();

        void Poll();
    };
}
