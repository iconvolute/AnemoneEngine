#pragma once

namespace Anemone::Network
{
    struct PosixNetworkStatic : GenericNetworkStatic
    {
        static void Initialize();
        static void Finalize();
    };

    using NetworkStatic = PosixNetworkStatic;
}
