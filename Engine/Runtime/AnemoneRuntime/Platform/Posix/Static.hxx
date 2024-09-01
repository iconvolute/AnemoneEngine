#pragma once

namespace Anemone::Platform
{
    struct PosixPlatformStatic : GenericPlatformStatic
    {
        static void Initialize();
        static void Finalize();
    };

    using PlatformStatic = PosixPlatformStatic;
}
