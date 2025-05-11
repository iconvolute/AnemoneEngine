#pragma once
#include "AnemoneRuntime/App/ISplashScreen.hxx"
#include "AnemoneRuntime/Interop/Windows/UI.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/Interop/Windows/SafeHandle.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"

namespace Anemone
{
    class SplashScreen final
        : public ISplashScreen
    {
    public:
        SplashScreen();
        SplashScreen(SplashScreen const&) = delete;
        SplashScreen(SplashScreen&&) = delete;
        SplashScreen& operator=(SplashScreen const&) = delete;
        SplashScreen& operator=(SplashScreen&&) = delete;
        ~SplashScreen() override;

    public:
        void SetText(std::string_view text) override;
        void SetProgress(float progress) override;

    private:
        Interop::Windows::SafeHandle _thread{};
        Interop::Windows::SafeHandle _sync{};
        ATOM _class{};
        HWND _window{};

        CriticalSection _lock{};
        Interop::string_buffer<wchar_t, 64> _text{};
        float _progress{0.0f};

    private:
        static LRESULT CALLBACK WindowProc(
            HWND hwnd,
            UINT msg,
            WPARAM wparam,
            LPARAM lparam);

        static DWORD WINAPI ThreadProc(LPVOID lpParameter);
    };
}
