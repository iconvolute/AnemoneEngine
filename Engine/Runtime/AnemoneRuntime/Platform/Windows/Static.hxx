#pragma once

namespace Anemone::Platform
{
    struct WindowsPlatformStatic : GenericPlatformStatic
    {
        static void Initialize();
        static void Finalize();
    };

    using PlatformStatic = WindowsPlatformStatic;
}
