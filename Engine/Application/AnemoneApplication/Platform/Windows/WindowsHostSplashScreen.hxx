#pragma once
#include "AnemoneApplication/HostSplashScreen.hxx"
#include "AnemoneInterop/Windows/UI.hxx"
#include "AnemoneInterop/Windows/Text.hxx"
#include "AnemoneInterop/Windows/SafeHandle.hxx"
#include "AnemoneThreading/CriticalSection.hxx"

namespace Anemone
{
    class WindowsHostSplashScreen final
        : public HostSplashScreen
    {
    public:
        WindowsHostSplashScreen();
        WindowsHostSplashScreen(WindowsHostSplashScreen const&) = delete;
        WindowsHostSplashScreen(WindowsHostSplashScreen&&) = delete;
        ~WindowsHostSplashScreen() override;
        WindowsHostSplashScreen& operator=(WindowsHostSplashScreen const&) = delete;
        WindowsHostSplashScreen& operator=(WindowsHostSplashScreen&&) = delete;

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
