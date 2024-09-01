#pragma once
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#include <netlistmgr.h>
#include <wrl/client.h>

#include <optional>

namespace Anemone::Network
{
    struct WindowsNetworkStatic : GenericNetworkStatic
    {
        Microsoft::WRL::ComPtr<INetworkListManager> NetworkListManager;

        std::optional<bool> HasInternetConnection() const;

        static void Initialize();
        static void Finalize();
    };

    extern UninitializedObject<WindowsNetworkStatic> GNetworkStatic;

    using NetworkStatic = WindowsNetworkStatic;
}
