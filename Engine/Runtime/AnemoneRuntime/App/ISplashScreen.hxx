#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#include <string_view>

namespace Anemone
{
    class ISplashScreen
    {
    public:
        ISplashScreen() = default;
        ISplashScreen(ISplashScreen const&) = delete;
        ISplashScreen(ISplashScreen&&) = delete;
        ISplashScreen& operator=(ISplashScreen const&) = delete;
        ISplashScreen& operator=(ISplashScreen&&) = delete;
        virtual ~ISplashScreen() = default;

    public:
        RUNTIME_API static void Initialize();
        RUNTIME_API static void Finalize();
        RUNTIME_API static ISplashScreen& Get();

    public:
        virtual void SetText(std::string_view text) = 0;
        virtual void SetProgress(float progress) = 0;
    };
}
